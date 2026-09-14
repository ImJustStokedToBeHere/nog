cmake_minimum_required(VERSION 3.8)

project(
  "nog"
  VERSION 0.0.1
  LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 23) set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SCRATCH_ONLY
       "build only the scratch executable, exclude building and linking NOG" ON)

option(BUILD_SHARED_LIBS "build zog as a shared_library" ON)
option(
  IN_DEVELOPMENT
  "use this flag for allowing console output from anywhere for simple debugging while actually writing code"
  OFF)

option(NOG_TESTS_ONLY
       "build only the nog_tests executable, exclude rebuilding NOG" ON)

# enable_testing()
#
# # 1. Fetch GoogleTest from GitHub include(FetchContent) FetchContent_Declare(
# googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG
# v1.15.2 # Use a stable tag or release ) # For Windows compatibility: prevents
# GTest from overriding compiler runtime # settings set(GTEST_FORCE_SHARED_CRT
# ON CACHE BOOL "" FORCE) FetchContent_MakeAvailable(googletest)

# Include sub-projects.
add_subdirectory("nog")
add_subdirectory("nogcl")
add_subdirectory("scratch")
# add_subdirectory("test")
