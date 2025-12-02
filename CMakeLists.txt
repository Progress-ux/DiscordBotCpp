cmake_minimum_required(VERSION 3.16)
project(bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Оптимизация
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -march=x86-64-v3)

# ===========================================================
# Fetch DPP
# ===========================================================
include(FetchContent)
FetchContent_Declare(
    dpp
    GIT_REPOSITORY https://github.com/brainboxdotcc/DPP.git
    GIT_TAG master
)
FetchContent_MakeAvailable(dpp)

find_package(CURL REQUIRED)

# ===========================================================
# Sources
# ===========================================================
file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories(include)

add_executable(bot ${SOURCES})

target_link_libraries(bot dpp CURL::libcurl)
