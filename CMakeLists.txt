cmake_minimum_required(VERSION 3.20)
project(pocket_cube_cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fexec-charset=GBK -finput-charset=UTF-8 -fwide-exec-charset=GBK") #GBK
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(pocket_cube main.cpp)
