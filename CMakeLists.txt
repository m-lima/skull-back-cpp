cmake_minimum_required(VERSION 3.10)
project(skull CXX)

##------------------------------------------------------------------------------
## Initialization
##

include("${CMAKE_SOURCE_DIR}/Init.cmake")

##------------------------------------------------------------------------------
## Dependencies
##

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
      "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_install(
    BUILD missing
    CONANFILE conanfile.txt
)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

list(APPEND LIBRARIES CONAN_PKG::mfl)
list(APPEND LIBRARIES CONAN_PKG::restinio)

##------------------------------------------------------------------------------
## Sources
##

list(APPEND SOURCES
  ${SRC_DIR}/server.cpp
)

list(APPEND HEADERS
  ${SRC_DIR}/server.hpp
  ${SRC_DIR}/quick_values.hpp
)

##------------------------------------------------------------------------------
## Targets
##

add_definitions(${DEFINITIONS})

add_library(skull-lib STATIC ${SOURCES})
target_link_libraries(skull-lib PUBLIC ${LIBRARIES})
target_include_directories(skull-lib PUBLIC ${INCLUDE_DIRS})

# Copy the resources
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/res)
file(GLOB RESOURCES ${SRC_DIR}/res/*)
file(COPY ${RESOURCES} DESTINATION ${CMAKE_BINARY_DIR}/bin/res)

##------------------------------------------------------------------------------
## Main
##

add_executable(skull ${SRC_DIR}/main.cpp)
target_link_libraries(skull PRIVATE skull-lib)

##------------------------------------------------------------------------------
## Tests
##

include("${CMAKE_SOURCE_DIR}/Tests.cmake")
