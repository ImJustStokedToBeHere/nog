add_executable(nogcl "main.cpp")

message(STATUS "BUILDING: nogcl")

set_target_properties(nogcl PROPERTIES OUTPUT_NAME "nogcl" CXX_STANDARD 20)

# TODO: Add tests and install targets if needed. set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if(MSVC_VERSION GREATER_EQUAL "1900") include(CheckCXXCompilerFlag)
# check_cxx_compiler_flag("/std:c++20" _cpp_latest_flag_supported)
# if(_cpp_latest_flag_supported) message(STATUS " nogcl: cpp latest flag
# supported") set_property(TARGET nogcl PROPERTY CXX_STANDARD 20) endif()
# endif()

message(STATUS " nogcl: nogcl CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

target_include_directories(nogcl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_BASE_DEFINITIONS "$<$<CONFIG:Debug>:" "_DEBUG" ">" NOMINMAX)

target_compile_definitions(nogcl PRIVATE ${LIB_BASE_DEFINITIONS})

# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/version.h.in"
# "${CMAKE_CURRENT_SOURCE_DIR}/version.h")

message(STATUS " nogcl: CMAKE_CXX_COMPILER_LOADED: "
               ${CMAKE_CXX_COMPILER_LOADED})
message(STATUS " nogcl: CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
message(STATUS " nogcl: CMAKE_CXX_COMPILER_VERSION: "
               ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS " nogcl: CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(STATUS " nogcl: using Clang")

  # set_target_properties(nogcl PROPERTIES LINK_FLAGS "/lldmap:nogcl.map")

  # set_target_properties( nogcl PROPERTIES LINK_FLAGS
  # "-W,-Map=${CMAKE_BINARY_DIR}/nogcl.map") target_link_options(nogcl PRIVATE
  # "/lldmap:${CMAKE_BINARY_DIR}/nogcl.map")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC message(STATUS " nogcl: using GCC")
  message(STATUS " nogcl: CMAKE_COMPILER_IS_GNUCXX: "
                 ${CMAKE_COMPILER_IS_GNUCXX})
  # set_target_properties( nogcl PROPERTIES LINK_FLAGS
  # "-Wl,-Map=${CMAKE_BINARY_DIR}/nogcl.map")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++ message(STATUS " nogcl: using Intel C++")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++ message(STATUS " nogcl: using Visual Studio C++")

  # set_target_properties(nogcl PROPERTIES LINK_FLAGS "-MAP:nogcl.map")
endif()

target_link_libraries(nogcl PRIVATE nog)
