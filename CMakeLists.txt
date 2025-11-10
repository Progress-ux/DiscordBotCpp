cmake_minimum_required(VERSION 3.15)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FFMPEG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/libs/ffmpeg/ffmpeg_build)

# DPP
add_subdirectory(libs/DPP)

# OGG
add_subdirectory(libs/ogg)

# Opusfile
add_subdirectory(libs/opusfile)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/DPP/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/opusfile/include
    ${FFMPEG_ROOT}/include
)

# RPATH чтобы бинарник видел .so
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_RPATH "${FFMPEG_ROOT}/lib"
)

# Opus
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPUS REQUIRED opus)
target_include_directories(${PROJECT_NAME} PRIVATE ${OPUS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
   dpp
   opusfile
   ogg
   ${OPUS_LIBRARIES}

   ${FFMPEG_ROOT}/lib/libswscale.so
   ${FFMPEG_ROOT}/lib/libswresample.so
   ${FFMPEG_ROOT}/lib/libavformat.so
   ${FFMPEG_ROOT}/lib/libavcodec.so
   ${FFMPEG_ROOT}/lib/libavutil.so
   ${FFMPEG_ROOT}/lib/libavdevice.so     
   ${FFMPEG_ROOT}/lib/libavfilter.so     
)
