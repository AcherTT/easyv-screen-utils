# 哎。没完明白，暂时用不了，貌似我本地node位置需要声明。
cmake_minimum_required(VERSION 3.15)
project(screen_utils_native)

# 设置 CMake 版本和策略
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0091 NEW)

add_definitions(-DNAPI_VERSION=4)

# 设置 C++ 标准版本
set(CMAKE_CXX_STANDARD 17)

# 设置 node 模块路径
set(NodeJS_DIR /home/tantan/.config/nvm/versions/node/v18.15.0/bin)

# 查找需要的库
find_package(NodeJS REQUIRED)
find_package(napi REQUIRED)

# 添加头文件搜索路径
include_directories(${CMAKE_JS_INC})
# include_directories(${NodeJS_INCLUDE_DIRS})
# include_directories(${napi_INCLUDE_DIRS})

# 添加源代码文件
file(GLOB SOURCE_FILES "src/*.cc" "src/*.h" "src/db_pool/*.cc" "src/db_pool/*.h" "src/include/rapidjson/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

# 添加目标文件头文件搜索路径
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
# target_include_directories(${PROJECT_NAME} PRIVATE ${NodeJS_INCLUDE_DIRS})
# target_include_directories(${PROJECT_NAME} PRIVATE ${napi_INCLUDE_DIRS})

# 链接所需的库
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
# target_link_libraries(${PROJECT_NAME} ${NodeJS_LIBRARIES})
# target_link_libraries(${PROJECT_NAME} ${napi_LIBRARIES})

# 定义所需的宏和变量
add_definitions(-DNAPI_VERSION=${NAPI_VERSION})

# 输出目标文件
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()