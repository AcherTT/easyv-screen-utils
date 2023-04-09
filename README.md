# EasyV 大屏导入工具

**目前还没在windows系统中编译过，一个是因为依赖没有集成到项目内部，安装依赖比较麻烦，另一个原因是我暂时手里没有Windows电脑**

## 需求背景
JSON.parse 解析 json 时，内存占用较大，且 js 层业务代码并不关心解析出来的具体值，只需要简单替换引用关系并存入数据库即可。

## 解决方案
我们使用 c++ 扩展来替换原来的导入方法。
### 优点
1. 速度和内存占用完全不需要忧虑，理论上可以实现极致的性能和最小的内存占用。
2. 不需要再开一个服务，仅作为一个node模块，灵活调用。
### 缺点
1. c/c++代码的维护成比ts本高很多。
2. 不安全，容易出现内存泄露，主要是由于c++的内存管理很复杂，非常考验程序员的对c/c++的理解深度。
3. 兼容性问题，不同的系统，不同的依赖版本，甚至不同的v8版本都可能导致插件失效，虽然node-api号称屏蔽了node和v8版本。
4. 自己实现的数据库连接池过于简单，目前只实现了线程安全，但是断开重连等各种机制没有实现，这是一个头疼的问题，后续要参考市面上成熟的连接池代码。

### 启动阶段
该扩展插件可以注册到 nestjs 的 provider，并传入数据库连接的配置，扩展实例化时创建数据库连接池。

### 运行阶段
1. 前端将包文件传到后端服务器
2. js 层调用扩展的导入函数，传入磁盘上文件的地址，
3. 扩展层解压读取文件，解析 json，然后执行业务逻辑，这段执行是在 nodejs 自带的 libuv 线程池中，不产生额外系统消耗。
4. 后续可以调研并阅读一下js层的数据库驱动代码，看能不能不自己弄一套连接池，因为自己写的不稳定。


## 依赖
1. rapidjson: 解析json，不需要安装，集成在项目里。
2. libpq: 数据库连接工具，安装方式如下
   ```sh
    sudo apt-get install libpq-dev
    sudo yum install postgresql-devel
    brew install libpq
   ```
3. zlib
   ```sh
    sudo apt-get install libzip-dev
    sudo yum install libzip-devel
    brew install libzip
   ```
4. node-gyp: 编译工具
   安装方式参考：[github连接](https://github.com/nodejs/node-gyp)

## 编译
### 类Unix系统
需要注意不同系统在安装完libpq后，可能会有 postgresql 头文件位置不同的情况，如有不同请修改 binding.gyp 的 include_dirs
```sh
npm run build
```


## Q&A
1. 为什么使用 rapidjson？
    在综合对比 c/c++ 市面上所有 json 解析工具之后，最终决定使用 rapidjson，优点是性能高内存小，不过 api 设计不合理，难用。
2. 为什么 c++ 层是解析文件，而不是 js 层将字符串传递到c++层？
    这个是临时方案，首先如果数据量很大，js 层不需要关心具体内容，直接存在磁盘即可，否则内存会被无意义的消耗。其次经过测试，c++ 层读取 160 mb 的文件并解析成 document 时，速度也可接受。

## TODO

- [x] 使用异步模式，将任务丢到 libuv。
- [x] 创建数据库连接池，考虑 libpq、libpqxx。
- [x] 将所有文件内容解析成 json document。
- [ ] 实现导入大屏完整逻辑，剩下遍历数据，然后存入数据库。
- [ ] 封装成 nestjs 模块。
- [ ] 数据一次性加载改为按需加载，这样可以更加节省内存，且性能不变。
- [ ] 性能测试 tinybench。
- [ ] 实现数据库连接池更完整的功能，否则不稳定。
- [ ] 考虑使用 orm 框架增加项目可维护性 SOCI。
- [ ] 使用 CmakeJs 替换 node-gyp，Google 已经不经常维护 node-gyp 了。
- [ ] 尝试将 libzip 与 libpq 集成到项目，做到不依赖系统。
- [ ] 单元测试。


## 临时小记 - 非常简单的测试了一下时间内存，后续考虑更详细的测试
1. 测试机器：ubuntu 16（deepin 20），6核12线程，3.6GHz。
2. 大屏文件：panel.json 50MB，component.json 50MB，其他较小忽略不计。
3. 时间花费：从开始到将所有文件数据解析成json document的时间在300ms以内。后续如果不在乎api易用性的话，可以使用rapidjson的sax模式，速度将会更快。
4. 内存消耗：运行的全程，最小内存51MB，最大到160MB，差距大约在110MB，基本就是包的大小。从rapidjson的文档来看，除字符串类型，其他类型都占用16字节，和json在磁盘中占用空间接近。