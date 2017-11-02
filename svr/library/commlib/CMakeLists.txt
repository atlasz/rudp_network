cmake_minimum_required(VERSION 2.8)

file(GLOB SRC_LIST *.cc *.cpp */*.cpp)

add_library(comm STATIC ${SRC_LIST})