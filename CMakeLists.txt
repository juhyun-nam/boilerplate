cmake_minimum_required(VERSION 3.5.1)

project(template LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_UNIT_TESTS "Enable unit tests" "ON")
#option(WITH_JEMALLOC "build with JeMalloc" OFF)
#option(WITH_SNAPPY "build with SNAPPY" OFF)
#option(WITH_LZ4 "build with lz4" OFF)
#option(WITH_ZLIB "build with zlib" OFF)
#option(WITH_ZSTD "build with zstd" OFF)

find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(gflags_external
  PREFIX gflags
  SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/gflags"
  CMAKE_CACHE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/gflags
)
ExternalProject_Get_Property(gflags_external install_dir)
set(GFLAGS_INCLUDE_DIR ${install_dir}/include)
file(MAKE_DIRECTORY ${GFLAGS_INCLUDE_DIR})
add_library(gflags STATIC IMPORTED)
set_target_properties(gflags PROPERTIES
  IMPORTED_LOCATION "${install_dir}/lib/libgflags.a"
  INTERFACE_INCLUDE_DIRECTORIES "${GFLAGS_INCLUDE_DIR}"
)
add_dependencies(gflags gflags_external)
ExternalProject_Add(googletest
  PREFIX googletest
  SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/googletest"
  CMAKE_CACHE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/googletest
  EXCLUDE_FROM_ALL  1
)
ExternalProject_Get_Property(googletest install_dir)
set(GTEST_INCLUDE_DIR ${install_dir}/include)
file(MAKE_DIRECTORY ${GTEST_INCLUDE_DIR})
add_library(gtest STATIC IMPORTED)
add_library(gtest_main STATIC IMPORTED)
set_target_properties(gtest PROPERTIES
  IMPORTED_LOCATION "${install_dir}/lib/libgtest.a"
  INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
  INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
)
set_target_properties(gtest_main PROPERTIES
  IMPORTED_LOCATION "${install_dir}/lib/libgtest_main.a"
  INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
  INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
)
add_dependencies(gtest googletest)
add_dependencies(gtest_main googletest)

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

add_library(simple_web_server INTERFACE)
target_include_directories(simple_web_server INTERFACE
  third_party/Simple-Web-Server
)

add_library(template_internal STATIC
)
set(DEBUG_COMPILE_FLAGS -Wall -Wextra -Werror)
target_compile_options(template_internal PUBLIC $<$<CONFIG:Debug>:${DEBUG_COMPILE_FLAGS}>)
target_link_libraries(template_internal PUBLIC
  simple_web_server
  spdlog
  GSL
  json
  gflags
  ${CMAKE_THREAD_LIBS_INIT}
)
target_include_directories(template_internal PUBLIC
  ${PROJECT_SOURCE_DIR}
)
add_executable(template
  template/main.cc
)
target_link_libraries(template PRIVATE
  template_internal
)

if (ENABLE_UNIT_TESTS)
  find_program(MEMORYCHECK_COMMAND NAME valgrind)
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
  include(CTest)
  enable_testing()
  set(TEST_SOURCES
  )
  add_executable(unit_test
    ${TEST_SOURCES}
  )
  target_link_libraries(unit_test PRIVATE
    template_internal
    gtest
    gtest_main
    GSL
  )
  add_test(NAME unit_test
    COMMAND $<TARGET_FILE:unit_test>
  )
endif()

include(GNUInstallDirs)
install(TARGETS template
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
