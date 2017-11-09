cmake_minimum_required(VERSION 2.8)

message(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    .
    ${PROJECT_ROOT_DIR}/library/
    ${CMAKE_CURRENT_BINARY_DIR}
    )

LINK_DIRECTORIES(
	${LIBRARY_OUTPUT_PATH}
)

file(GLOB SRC_LIST *.cpp)

add_executable(first ${SRC_LIST})

TARGET_LINK_LIBRARIES(
    first
    comm
    networking
)