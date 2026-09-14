cmake_minimum_required(VERSION 3.8)

project(
  "nog"
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

option(SCRATCH_ONLY
       "build only the scratch executable, exclude building and linking NOG" ON)
option(BUILD_SHARED_LIBS "build zog as a shared_library" OFF)
option(
  IN_DEVELOPMENT
  "use this flag for allowing console output from anywhere for simple debugging while actually writing code"
  OFF)

# Include sub-projects.
add_subdirectory("nog")
add_subdirectory("nogcl")
add_subdirectory("scratch")
