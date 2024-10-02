cmake_minimum_required(VERSION 3.1.0)

project(RTWeekend LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_FLAGS_RELEASE "-O0 -g -DNDEBUG")
set(CMAKE_BUILD_TYPE "Release")

set ( SOURCE_ONE_WEEKEND
  src/InOneWeekend/main.cpp
  src/InOneWeekend/hittable.h
  src/InOneWeekend/sphere.h
  src/InOneWeekend/hittable_list.h
  src/InOneWeekend/global.h
  src/InOneWeekend/vec3.h
  src/InOneWeekend/color.h
  src/InOneWeekend/ray.h
  src/InOneWeekend/interval.h
)

include_directories(src)

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message (STATUS "Release flags: " ${CMAKE_CXX_FLAGS_RELEASE})
message (STATUS "Debug flags: " ${CMAKE_CXX_FLAGS_DEBUG})

add_executable(InOneWeekend ${SOURCE_ONE_WEEKEND})
