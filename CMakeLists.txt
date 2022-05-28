cmake_minimum_required(VERSION 3.19)
project(VRVolumeRenderer)

set(CMAKE_CXX_STANDARD 14)

set(PROJECT_THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third-party)
#openvr
set(OPENVR_INCLUDE_DIR ${PROJECT_THIRD_PARTY_DIR}/openvr/include)
if(WIN32)
    set(OPENVR_BINARY_DIR ${PROJECT_THIRD_PARTY_DIR}/openvr/bin/win64)
elseif(UNIX AND NOT APPLE)
    set(OPENVR_BINARY_DIR ${PROJECT_THIRD_PARTY_DIR}/openvr/bin/linux64)
endif()
#glad
set(GLAD_INCLUDE_DIR ${PROJECT_THIRD_PARTY_DIR}/glad/include)
set(GLAD_SRC_DIR ${PROJECT_THIRD_PARTY_DIR}/glad/src)
#glm
set(GLM_INCLUDE_DIR ${PROJECT_THIRD_PARTY_DIR}/glm/glm)
#sdl2-2.0.3
set(SDL2_INCLUDE_DIR ${PROJECT_THIRD_PARTY_DIR}/sdl2-2.0.12/include)
#if(WIN32)
#    set(SDL2_BINARY_DIR ${PROJECT_THIRD_PARTY_DIR}/sdl2-2.0.3/bin/win64)
#elseif(UNIX AND NOT APPLE)
#    set(SDL2_BINARY_DIR ${PROJECT_THIRD_PARTY_DIR}/sdl2-2.0.3/bin/linux64)
#endif()
#include_directories(${SDL2_INCLUDE_DIR})
add_subdirectory(src)
add_subdirectory(third-party/SDL2-2.0.12)