cmake_minimum_required(VERSION 3.6)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")


# 添加AVX512F支持
# CMakeLists.txt
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx512f -mavx512cd")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512f -mavx512cd")

add_executable(src bitParellMatch.cpp
bitSequence.cpp
bitSequence.h
event.h
fileio.cpp
fileio.h
global.cpp
global.h
includefile.h
Match.cpp
Match.h
parseData.cpp
parseData.h
parseQuery.cpp
parseQuery.h
parseResult.cpp
parseResult.h
)

