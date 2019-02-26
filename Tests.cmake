if (MAKE_TESTS)
  message(STATUS "Making tests")

  enable_testing()
  include(GoogleTest)

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
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
  )

  # CTest integration
  gtest_discover_tests(skull-test)

  # Copy the resources
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/res)
  file(GLOB RESOURCES ${TEST_DIR}/res/*)
  file(COPY ${RESOURCES} DESTINATION ${CMAKE_BINARY_DIR}/test/res)

  # Run tests before main build
  add_custom_target(test-all COMMAND ./test/skull-test --gtest_shuffle)
  add_dependencies(test-all skull-test)
  add_dependencies(skull test-all)
endif()

