cmake_minimum_required(VERSION 3.28)

if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(
    FATAL_ERROR
      "In-source builds are disabled.
       Please create a subfolder and use `cmake ..` inside it.
       NOTE: cmake will now create CMakeCache.txt and CMakeFiles/*.
       You must delete them, or cmake will refuse to work.")
endif()

project(cpp-master
  LANGUAGES CXX
  DESCRIPTION "C++ master course hometasks"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)
include(cmake/cmake-flags.cmake)

CPMAddPackage("gh:ericniebler/range-v3#0.12.0")
CPMAddPackage("gh:fmtlib/fmt#11.0.2")
CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG release-1.12.1
  VERSION 1.12.1
  OPTIONS "INSTALL_GTEST OFF"
)

add_library(cpp-master-settings INTERFACE)
target_compile_features(cpp-master-settings INTERFACE cxx_std_23)
target_link_libraries(cpp-master-settings INTERFACE range-v3)
target_link_libraries(cpp-master-settings INTERFACE fmt)
apply_compiler_flags(cpp-master-settings INTERFACE)

enable_testing()

add_subdirectory(l1)
