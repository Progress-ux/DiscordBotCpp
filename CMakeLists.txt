# CMakeLists.txt (Верхний уровень)

cmake_minimum_required(VERSION 3.15)
project(OverallProject LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# DPP
add_subdirectory(libs/DPP)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(main ${SRC_FILES})

target_include_directories(main
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/libs/DPP/include
)

target_link_libraries(main PRIVATE dpp)