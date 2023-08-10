  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  # Add a GoogleTest test executable for each test file:
  add_executable(
    graph_reader_test
    graph_reader_test.cpp
  )

  add_executable(
    maxflow_test
    maxflow_test.cpp
  )

  # Link the GTest::gtest_main library to each test executable,
  # allowing the test to use GoogleTest's functions and classes.
  target_link_libraries(
    graph_reader_test
    GTest::gtest_main
  )

  target_link_libraries(
    maxflow_test
    GTest::gtest_main
  )

  # Include GoogleTest's CMake functions
  include(GoogleTest)

  # Add the tests to CTest's test runner
  gtest_discover_tests(graph_reader_test)
  gtest_discover_tests(maxflow_test)

  # Add the ../src directory to the include path for each test target,
  # making it possible to include header files located in that directory during compilation
  target_include_directories(graph_reader_test PRIVATE ../src)
  target_include_directories(maxflow_test PRIVATE ../src)

