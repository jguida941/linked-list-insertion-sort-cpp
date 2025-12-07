cmake_minimum_required(VERSION 3.30)
project(linked_list_insertion_sort)

set(CMAKE_CXX_STANDARD 20)

add_executable(linked_list_insertion_sort src/main.cpp)

# Add include directory for headers
target_include_directories(linked_list_insertion_sort PRIVATE include)

# Optional: enable TRACE to print the list after key steps during sorting
option(TRACE "Enable trace logging" OFF)
if(TRACE)
    target_compile_definitions(linked_list_insertion_sort PRIVATE TRACE)
endif()
