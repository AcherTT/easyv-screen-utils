# EasyV 大屏导入工具

## 需求背景
JSON.parse 解析 json 时，内存占用较大，且 js 层业务代码并不关心解析出来的具体值，只需要简单替换引用关系并存入数据库即可。

## 解决方案
我们使用 c++ 扩展来替换原来的导入方法。

### 启动阶段
该扩展插件可以注册到 nestjs 的 provider，并传入数据库连接的配置，扩展实例化时创建数据库连接池。

### 运行阶段
1. 前端将包文件传到后端服务器
2. js 层调用扩展的导入函数，传入磁盘上文件的地址，
3. 扩展层读取文件，解析 json，然后执行业务逻辑，这段执行是在 nodejs 自带的 libuv 线程池中，不产生额外系统消耗。


## 依赖
1. rapidjson: 解析json，不需要安装，集成在项目里。
2. libpq: 数据库连接工具，安装方式如下
   ```sh
    sudo apt-get install libpq-dev
    sudo yum install postgresql-devel
    brew install libpq
   ```
3. node-gyp: 编译工具
   安装方式参考：[github连接](https://github.com/nodejs/node-gyp)

## Q&A
1. 为什么使用 rapidjson？
    在综合对比 c/c++ 市面上所有 json 解析工具之后，最终决定使用 rapidjson，优点是性能高内存小，确定是 api 设计不合理，难用。
2. 为什么 c++ 层是解析文件，而不是 js 层将字符串传递到c++层？
    这个是临时方案，首先如果数据量很大，js 层不需要关心具体内容，直接存在磁盘即可，否则内存会被无意义的消耗。其次经过测试，c++ 层读取 160 mb 的文件并解析成 document 时，速度也可接受。

## TODO

- [x] 使用异步模式，将任务丢到 libuv。
- [ ] 创建数据库连接池，考虑 libpq、libpqxx。
- [ ] 实现导入大屏完整逻辑。
- [ ] 封装成 nestjs 模块。
- [ ] 考虑使用 orm 框架增加项目可维护性 SOCI。
- [ ] 使用 CmakeJs 替换 node-gyp，纯属个人喜好，因为 cmake 才是主流，且 Google 已经不经常维护 node-gyp 了。
- [ ] 单元测试。
- [ ] 性能测试。
