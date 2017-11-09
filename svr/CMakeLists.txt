cmake_minimum_required(VERSION 2.8)

project(NetworkCode)

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

MESSAGE(STATUS "This is PROJECT_ROOT dir " ${PROJECT_ROOT_DIR})
MESSAGE(STATUS "This is BINARY dir " ${HELLO_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir " ${HELLO_SOURCE_DIR})

#set(CMAKE_BUILD_TYPE Debug) # 没有效果 下面一行 -DDEBUG有效
#add_definitions(-DSELF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -std=c++11 -fPIC ")


set(LIBRARY_OUTPUT_PATH ${PROJECT_ROOT_DIR}/dist/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_ROOT_DIR}/dist/bin)

MESSAGE(STATUS "This is EXCUTABLE_OUTPUT_PATH dir " ${EXCUTABLE_OUTPUT_PATH})

INCLUDE_DIRECTORIES(
     ${PROJECT_ROOT_DIR}/library
     ${PROJECT_ROOT_DIR}/library/commlib
  	 ${PROJECT_ROOT_DIR}/library/networking
)

LINK_DIRECTORIES(
	${LIBRARY_OUTPUT_PATH}
)
#${THIRDPARTY_DIR}/dist/lib

#add lib subdirectory
add_subdirectory(library/commlib)
add_subdirectory(library/networking)

#add bin subdirectory
add_subdirectory(service/test)
add_subdirectory(service/first)