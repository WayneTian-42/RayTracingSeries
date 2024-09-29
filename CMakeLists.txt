cmake_minimum_required(VERSION 3.1.0)

project(RTWeekend LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set ( SOURCE_ONE_WEEKEND
  src/InOneWeekend/main.cpp
  src/InOneWeekend/hittable.h
  src/InOneWeekend/sphere.h
  src/InOneWeekend/hittable_list.h
  src/InOneWeekend/global.h
  src/InOneWeekend/vec3.h
  src/InOneWeekend/color.h
  src/InOneWeekend/ray.h
)

include_directories(src)

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message (STATUS "Release flags: " ${CMAKE_CXX_FLAGS_RELEASE})
message (STATUS "Debug flags: " ${CMAKE_CXX_FLAGS_DEBUG})

add_executable(InOneWeekend ${SOURCE_ONE_WEEKEND})
