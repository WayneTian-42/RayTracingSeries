cmake_minimum_required(VERSION 3.1.0)

project(RTNextWeek LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "Release")

set ( SOURCE_NEXT_WEEK
  src/NextWeek/main.cpp
)

include_directories(src)

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message (STATUS "Release flags: " ${CMAKE_CXX_FLAGS_RELEASE})
message (STATUS "Debug flags: " ${CMAKE_CXX_FLAGS_DEBUG})

add_executable(NextWeek ${SOURCE_NEXT_WEEK})
