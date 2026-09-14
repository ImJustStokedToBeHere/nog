add_executable(scratch "main.cpp")

message(STATUS "BUILDING: scratch")

set_target_properties(scratch PROPERTIES OUTPUT_NAME "scratch" CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if(MSVC_VERSION GREATER_EQUAL "1900") include(CheckCXXCompilerFlag)
# check_cxx_compiler_flag("/std:c++20" _cpp_latest_flag_supported)
# if(_cpp_latest_flag_supported) message(STATUS " scratch: cpp latest flag
# supported") set_property(TARGET scratch PROPERTY CXX_STANDARD 20) endif()
# endif()

message(STATUS " scratch: scratch CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

target_include_directories(scratch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_BASE_DEFINITIONS "$<$<CONFIG:Debug>:" "_DEBUG" ">" NOMINMAX)

target_compile_definitions(scratch PRIVATE ${LIB_BASE_DEFINITIONS})

# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/version.h.in"
# "${CMAKE_CURRENT_SOURCE_DIR}/version.h")

message(STATUS " scratch: CMAKE_CXX_COMPILER_LOADED: "
               ${CMAKE_CXX_COMPILER_LOADED})
message(STATUS " scratch: CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
message(STATUS " scratch: CMAKE_CXX_COMPILER_VERSION: "
               ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS " scratch: CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(STATUS " scratch: using Clang")

  # set_target_properties(scratch PROPERTIES LINK_FLAGS "/lldmap:scratch.map")

  set_target_properties(
    scratch PROPERTIES LINK_FLAGS "-Wl,-Map=${CMAKE_BINARY_DIR}/scratch.map")
  # target_link_options(scratch PRIVATE
  # "/lldmap:${CMAKE_BINARY_DIR}/scratch.map")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC message(STATUS " scratch: using GCC")
  message(STATUS " scratch: CMAKE_COMPILER_IS_GNUCXX: "
                 ${CMAKE_COMPILER_IS_GNUCXX})
  set_target_properties(scratch PROPERTIES LINK_FLAGS "-Wl -map,scratch.map")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++ message(STATUS " scratch: using Intel C++")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++ message(STATUS " scratch: using Visual Studio C++")

  set_target_properties(scratch PROPERTIES LINK_FLAGS "-MAP:scratch.map")
endif()

if(NOT SCRATCH_ONLY)
  target_link_libraries(scratch PRIVATE nog)
endif()
