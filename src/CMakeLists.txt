set(SOURCES
    maxflow.cpp
)

add_executable(maxflow ${SOURCES})

file(COPY ${CMAKE_SOURCE_DIR}/graphExample.txt
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(maxflow PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})