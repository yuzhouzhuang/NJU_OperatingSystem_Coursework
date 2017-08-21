cmake_minimum_required(VERSION 3.6)
project(MemeryManage)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES call.h bottom.h bottom.c test.c type.h call.c MemeryPage.c MemeryPage.h)

add_executable(MemeryManage ${SOURCE_FILES})
