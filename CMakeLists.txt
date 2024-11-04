cmake_minimum_required(VERSION 3.9)

# project(RTNextWeek LANGUAGES CXX) 
project(RTRest LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_CXX_FLAGS "-pg")
set(CMAKE_CXX_FLAGS "-march=native -pthread")

# 查找 OpenMP 支持
# find_package(OpenMP)

# 如果找到了 OpenMP，设置编译选项
# if(OPENMP_FOUND)
#     # 设置编译器的 OpenMP 标志
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     # 或者，你也可以直接链接 OpenMP 库
#     # target_link_libraries(your_target_name PRIVATE OpenMP::OpenMP_C)
#     # target_link_libraries(your_target_name PRIVATE OpenMP::OpenMP_CXX)
# endif()

# include(CheckIPOSupported)
# check_ipo_supported(RESULT _IsIPOSupported)
#   if(_IsIPOSupported)
#   message(STATUS "Turn on INTERPROCEDURAL_OPTIMIZATION")
#   set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
# endif()

# set ( SOURCE_NEXT_WEEK
#   src/NextWeek/main.cpp
# )

set ( SOURCE_REST
    src/Rest/main.cpp
)

include_directories(src)

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message (STATUS "Release flags: " ${CMAKE_CXX_FLAGS_RELEASE})
message (STATUS "Debug flags: " ${CMAKE_CXX_FLAGS_DEBUG})

# 设置最大堆栈
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=1000000000")

set(CMAKE_MAXIMUM_RECURSION_DEPTH 1000000)

# add_executable(NextWeek ${SOURCE_NEXT_WEEK})
add_executable(Rest ${SOURCE_REST})
