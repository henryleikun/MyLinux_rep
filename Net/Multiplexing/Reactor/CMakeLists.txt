# 指定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(Reactor)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置可执行文件
add_executable(Reactor Main.cpp)
target_link_libraries(Reactor jsoncpp)

add_executable(Client CalClient.cpp)
target_link_libraries(Client jsoncpp)
