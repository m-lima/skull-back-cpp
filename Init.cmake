##------------------------------------------------------------------------------
## Initialization
##

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Test enabler
option(MAKE_TESTS "make tests" ON)

# Set release by default
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Enable folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Current state
if (WIN32)
  message(STATUS "WIN32")
endif ()
if (MSVC)
  message(STATUS "MSVC")
endif ()
if (APPLE)
  message(STATUS "APPLE")
endif ()

message(STATUS "Using C: " ${CMAKE_C_COMPILER_ID})
message(STATUS "Using CXX: " ${CMAKE_CXX_COMPILER_ID})

# Set high warning levels
if (MSVC)
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif ()
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic")
endif ()

# Make $HOME compatible with windows
STRING(REGEX REPLACE "\\\\" "/" ENV_HOME_DIR "$ENV{HOME}")

# Add custom modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/findcmake")

# Set base directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(EXT_DIR "${CMAKE_SOURCE_DIR}/ext")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(INC_DIR "${CMAKE_SOURCE_DIR}/inc")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
set(LIB_INSTALL_DIR "${CMAKE_BINARY_DIR}/lib")

# User aditional findpackage prefixes
set(USER_LIBS_PATH "${ENV_HOME_DIR}/lib" CACHE LIST "additional findpackage prefixes (~/lib)")
message(STATUS "Additional findpackage prefixes: " "${USER_LIBS_PATH}")

