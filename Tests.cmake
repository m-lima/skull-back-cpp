if (MAKE_TESTS)
  message(STATUS "Making tests")

  enable_testing()
  include(GoogleTest)

  set(TEST_BIN_DIR "${CMAKE_BINARY_DIR}/test")

  # GTest
  list(APPEND TEST_LIBRARIES
      CONAN_PKG::gtest
      skull-lib
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
  set_target_properties(skull-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TEST_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${TEST_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${TEST_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${TEST_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${TEST_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${TEST_BIN_DIR}"
  )

  # CTest integration
  gtest_discover_tests(skull-test)

  # Copy the resources
  file(MAKE_DIRECTORY ${TEST_BIN_DIR}/res)
  file(GLOB RESOURCES ${TEST_DIR}/res/*)
  file(COPY ${RESOURCES} DESTINATION ${TEST_BIN_DIR}/res)

  # Run tests before main build
  add_custom_target(test-all WORKING_DIRECTORY ${TEST_BIN_DIR} COMMAND ./skull-test --gtest_shuffle)
  add_dependencies(test-all skull-test)
  add_dependencies(skull test-all)
endif()

