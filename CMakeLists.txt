cmake_minimum_required(VERSION 3.5.1)

project(boilerplate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_UNIT_TESTS "Enable unit tests" "ON")
#option(WITH_JEMALLOC "build with JeMalloc" OFF)
#option(WITH_SNAPPY "build with SNAPPY" OFF)
#option(WITH_LZ4 "build with lz4" OFF)
#option(WITH_ZLIB "build with zlib" OFF)
#option(WITH_ZSTD "build with zstd" OFF)

if (ENABLE_UNIT_TESTS)
  find_package(GTest CONFIG REQUIRED)
  set(TEST_SOURCES
    boilerplate/tests/test.cc
  )
endif()

add_library(json INTERFACE)
target_include_directories(json INTERFACE
  third_party/json/single_include
)

add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE
  third_party/spdlog/include
)

add_library(GSL INTERFACE)
target_include_directories(GSL INTERFACE
  third_party/GSL/include
)

add_executable(main
  boilerplate/main.cc
)
target_link_libraries(main PRIVATE
  json
  spdlog
  GSL
)

if (ENABLE_UNIT_TESTS)
  find_package(GTest CONFIG REQUIRED)
  find_program(MEMORYCHECK_COMMAND NAME valgrind)
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
  include(CTest)
  enable_testing()
  set(TEST_SOURCES
    boilerplate/tests/test.cc
  )
  add_executable(main_test
    ${TEST_SOURCES}
  )
  target_link_libraries(main_test PRIVATE
    GTest::gtest_main
  )
  add_test(NAME main_test
    COMMAND $<TARGET_FILE:main_test>
  )
endif()
