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

include(cmake/cmake-flags.cmake)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

add_library(cpp-master-settings INTERFACE)
target_compile_features(cpp-master-settings INTERFACE cxx_std_23)
apply_compiler_flags(cpp-master-settings INTERFACE)

enable_testing()

add_subdirectory(l1)
