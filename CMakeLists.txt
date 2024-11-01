cmake_minimum_required(VERSION 3.1.0)

project(RTNextWeek LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Release")

# 查找 OpenMP 支持
find_package(OpenMP)

# 如果找到了 OpenMP，设置编译选项
if(OPENMP_FOUND)
    # 设置编译器的 OpenMP 标志
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    # 或者，你也可以直接链接 OpenMP 库
    # target_link_libraries(your_target_name PRIVATE OpenMP::OpenMP_C)
    # target_link_libraries(your_target_name PRIVATE OpenMP::OpenMP_CXX)
endif()

set ( SOURCE_NEXT_WEEK
  src/NextWeek/main.cpp
)

include_directories(src)

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message (STATUS "Release flags: " ${CMAKE_CXX_FLAGS_RELEASE})
message (STATUS "Debug flags: " ${CMAKE_CXX_FLAGS_DEBUG})

add_executable(NextWeek ${SOURCE_NEXT_WEEK})
