cmake_minimum_required(VERSION 3.14)
project(maxflow_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the source and test directories
add_subdirectory(src)
add_subdirectory(test)