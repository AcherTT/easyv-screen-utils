# 实验性功能，暂未测试是否可用

cmake_minimum_required(VERSION 3.10)

project(easyv_screen_utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

# Find Node.js
find_package(NodeJS REQUIRED)

# Include Node.js headers
execute_process(
    COMMAND node -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
include_directories(${NODE_ADDON_API_DIR})

# Add source files
set(SOURCES
    src/screen_utils.cc
    src/screen_importer.cc
)

# Create shared library target
add_library(easyv_screen_utils SHARED ${SOURCES})

# Set output filename to match the target name
set_target_properties(easyv_screen_utils PROPERTIES PREFIX "" SUFFIX ".node")

# Set include directories
target_include_directories(easyv_screen_utils PRIVATE ${CMAKE_JS_INC})

target_link_libraries(
    easyv_screen_utils
    PRIVATE
    ${CMAKE_JS_LIB}
    ${NODEJS_LIBRARIES}
    pq
)
