# CMakeList.txt : CMake project for frutec, include source and define project
# specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Add source to this project's executable.
option(PP_BUILD "build experimental preprocessor with source" OFF)
# option(LIB_BUILD "build jinn as a library")

set(HDR
    "build_def.h"
    "platform_def.h"
    "nog.h"
    "source.h"
    "assert.h"
    "error.h"
    "hash.h"
    "id.h"
    "macros.h"
    "version.h"
    "diagnostics.h"
    "util/time.h"
    "util/file.h"
    "util/types.h"
    "util/hash.h"
    "util/allocator.h"
    "enum_ops.h"
    "flag_ops.h"
    "syntax/symbol.h"
    "syntax/token.h"
    "syntax/ast.h"
    "syntax/parser.h")

list(TRANSFORM HDR PREPEND "include/nog/")

set(SRC "nog.cpp" "source.cpp" "util/time.cpp" "util/file.cpp"
        "diagnostics.cpp")

list(TRANSFORM SRC PREPEND "src/")

if(BUILD_SHARED_LIBS)
  set(LIB_TYPE "SHARED")
else()
  set(LIB_TYPE "STATIC")
endif()

add_library(nog ${LIB_TYPE} ${SRC} ${HDR})

message(STATUS "BUILDING: nog")

set_target_properties(nog PROPERTIES OUTPUT_NAME "nog" CXX_STANDARD 23)

# TODO: Add tests and install targets if needed.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if(MSVC_VERSION GREATER_EQUAL "1900") include(CheckCXXCompilerFlag)
# check_cxx_compiler_flag("/std:c++20" _cpp_latest_flag_supported)
# if(_cpp_latest_flag_supported) message(STATUS " nog: cpp latest flag
# supported") set_property(TARGET nog PROPERTY CXX_STANDARD 20) endif() endif()

message(STATUS " nog: CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

target_include_directories(
  nog
  INTERFACE $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:
            # ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/../
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            >
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(LIB_BASE_DEFINITIONS "$<$<CONFIG:Debug>:" "_DEBUG" ">" NOMINMAX)

if(IN_DEVELOPMENT)
  set(LIB_BASE_DEFINITIONS ${LIB_BASE_DEFINITIONS} "IN_DEVELOPMENT")
endif(IN_DEVELOPMENT)

if(PP_BUILD)
  set(LIB_BASE_DEFINITIONS ${LIB_BASE_DEFINITIONS} PP_BUILD)
endif(PP_BUILD)

if(BUILD_SHARED_LIBS)
  target_compile_definitions(nog PRIVATE ${LIB_BASE_DEFINITIONS} EXPORT)
else()
  target_compile_definitions(nog PRIVATE ${LIB_BASE_DEFINITIONS})
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/version.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/include/nog/version.h")

message(STATUS " nog: CMAKE_CXX_COMPILER_LOADED: " ${CMAKE_CXX_COMPILER_LOADED})
message(STATUS " nog: CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
message(STATUS " nog: CMAKE_CXX_COMPILER_VERSION: "
               ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS " nog: CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # using Clang message(STATUS " nog: using Clang")

  # set_target_properties(nog PROPERTIES CXX_STANDARD 17 LINK_FLAGS
  # "/lldmap:nog.map" )

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC message(STATUS " nog: using GCC")
  message(STATUS " nog: CMAKE_COMPILER_IS_GNUCXX: " ${CMAKE_COMPILER_IS_GNUCXX})
  # SET_TARGET_PROPERTIES(nog PROPERTIES LINK_FLAGS "-Wl -map,nog.map")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++ message(STATUS " nog: using Intel C++")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++ message(STATUS " nog: using Visual Studio C++")
  find_library(USERENV UserEnv)
  target_link_libraries(nog USERENV)
  # SET_TARGET_PROPERTIES(nog PROPERTIES LINK_FLAGS "-MAP:nog.map")
endif()
