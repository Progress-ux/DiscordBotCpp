cmake_minimum_required(VERSION 3.16)
project(MyBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Оптимизация
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -march=x86-64-v3)

# Fetch DPP
include(FetchContent)
FetchContent_Declare(
    dpp
    GIT_REPOSITORY https://github.com/brainboxdotcc/DPP.git
    GIT_TAG master
)
FetchContent_MakeAvailable(dpp)

# Sources
file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories(include)

# Define the executable first
add_executable(bot ${SOURCES})

# Link CURL 
find_package(CURL REQUIRED)
if(CURL_FOUND)
    # Use PRIVATE keyword
    target_link_libraries(bot PRIVATE ${CURL_LIBRARIES})
endif()

target_link_libraries(bot PRIVATE dpp) 

# Чтобы бинарник был переносимым
target_link_options(bot PRIVATE -static-libstdc++ -static-libgcc)
