cmake_minimum_required(VERSION 3.2)
project(algs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -g -O0")

set(SOURCE_FILES main.cpp)
add_executable(algs ${SOURCE_FILES} unionfind.h benchmark.h stack.h linkedlistnode.h queue.h sorts.h queue_policy_based.h 5algs.h priority_queue.h utils.h bst.h llrb.h hash_table.h threads.h applications/percolation.h simple_deque.h random_queue.h)

add_custom_command(TARGET algs POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:algs>/data)