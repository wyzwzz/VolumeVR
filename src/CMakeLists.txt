set(SHARED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shared)
file(GLOB SHARED_SRC_FILES
        ${SHARED_SRC_DIR}/*.cpp
        ${SHARED_SRC_DIR}/*.h
        )
if(UNIX AND NOT APPLE)
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++14 -include ${SHARED_SRC_DIR}/compat.h")
endif()
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}  SRC)

add_executable(VRVolumeRenderer ${SHARED_SRC_FILES}  ${SRC} ${GLAD_SRC_DIR}/glad.c)
add_definitions(-DLINUX -DPOSIX)
set(ARCH_TARGET linux64)
target_include_directories(VRVolumeRenderer PRIVATE
        ${GLAD_INCLUDE_DIR} ${OPENVR_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
find_library(OPENVR_LIBRARIES REQUIRED
        NAMES
        openvr_api
        PATHS
        ${OPENVR_BINARY_DIR}
        PATH_SUFFIXES
        win64
        linux64
        ${WINDOWS_PATH_SUFFIXES}
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
        )
target_link_libraries(VRVolumeRenderer PRIVATE
        ${OPENVR_LIBRARIES}   ${CMAKE_DL_LIBS} SDL2)