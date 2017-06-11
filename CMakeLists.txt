cmake_minimum_required(VERSION 3.1.0)

#==============================================================================#
# Project settings                                                             #
#==============================================================================#

project(unicodecpp)

option(UNICODECPP_BUILD_UNIT_TESTS "Build unit tests." OFF)

#==============================================================================#
# Compiler settings                                                            #
#==============================================================================#

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable all possible warnings
if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif(CMAKE_COMPILER_IS_GNUCC)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif(MSVC)

#==============================================================================#
# Core component                                                               #
#==============================================================================#

add_library(unicodecpp
  src/unicodecpp/v8_0_0/grapheme_segmentation.cpp)
target_include_directories(unicodecpp
  PUBLIC include
  PRIVATE src)

#==============================================================================#
# Unit tests                                                                   #
#==============================================================================#

if(UNICODECPP_BUILD_UNIT_TESTS)
  enable_testing()
  add_subdirectory(thirdparty/googletest)
  set(GTEST_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/googletest/include")

  add_executable(unicodecpp_unit_tests
    tests/unicodecpp/utf8_to_utf32_iterator_tests.cpp
    tests/unicodecpp/v8_0_0/grapheme_segmentation_tests.cpp)
  target_include_directories(unicodecpp_unit_tests
    PRIVATE ${GTEST_INCLUDE_DIRS})
  target_link_libraries(unicodecpp_unit_tests unicodecpp gmock_main)

  add_test(unicodecpp_unit_tests unicodecpp_unit_tests)
endif(UNICODECPP_BUILD_UNIT_TESTS)

