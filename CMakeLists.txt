cmake_minimum_required(VERSION 3.2)
project(algs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(algs ${SOURCE_FILES} unionfind.h benchmark.h stack.h linkedlistnode.h queue.h sorts.h queue_policy_based.h 5algs.h)

add_custom_command(TARGET algs POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:algs>/data)