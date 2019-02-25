if (MAKE_TESTS)
  message(STATUS "Making tests")

  enable_testing()
  include(GoogleTest)

  # GTest
  include("${EXT_DIR}/gtest/gtest.cmake")
  list(APPEND TEST_LIBRARIES
      skull-lib
      gtest_main
  )

  list(APPEND TEST_INCLUDE_DIRS "${SRC_DIR}")

  # Test sources
  list(APPEND TESTS
    ${TEST_DIR}/test_server.cpp
  )

  # Test executable
  add_executable(skull-test ${TEST_DIR}/test_main.cpp ${TESTS})
  target_link_libraries(skull-test PRIVATE ${TEST_LIBRARIES})
  target_include_directories(skull-test PRIVATE ${TEST_INCLUDE_DIRS})

  # CTest integration
  gtest_discover_tests(skull-test)

  # Copy the resources
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/res)
  file(GLOB RESOURCES ${TEST_DIR}/res/*)
  file(COPY ${RESOURCES} DESTINATION ${CMAKE_BINARY_DIR}/test/res)

  # Run tests before main build
  add_custom_target(test-all COMMAND ./bin/skull-test --gtest_shuffle)
  add_dependencies(test-all skull-test)
  add_dependencies(skull test-all)
endif()

