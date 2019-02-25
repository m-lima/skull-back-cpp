configure_file("${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt.in" "${CMAKE_CURRENT_LIST_DIR}/pack/pre/CMakeLists.txt")

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE failure
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/pack/pre")

if (failure)
  message(FATAL_ERROR "Failed to configure GTest")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE failure
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/pack/pre")

if (failure)
  message(FATAL_ERROR "Failed to make GTest")
endif()

# Prevent overriding the parent project compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines the gtest and gtest_main targets.
add_subdirectory(
    ${CMAKE_CURRENT_LIST_DIR}/pack/gtest
    ${CMAKE_BINARY_DIR}/deps/gtest
    EXCLUDE_FROM_ALL
)

set(INSTALL_GTEST OFF)

