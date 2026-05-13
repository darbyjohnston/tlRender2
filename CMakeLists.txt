cmake_minimum_required(VERSION 3.24)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0114 NEW)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

################################################################################
# vcpkg

# Initialize the vcpkg submodule if it hasn't been already.
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/vcpkg/.git")
    find_package(Git QUIET)
    if(Git_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        message(STATUS "Initializing vcpkg submodule...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE git_submod_result)
        if(NOT git_submod_result EQUAL 0)
            message(FATAL_ERROR
                "git submodule update failed. Run manually: "
                "git submodule update --init --recursive")
        endif()
    else()
        message(FATAL_ERROR
            "vcpkg submodule not found. Run: "
            "git submodule update --init --recursive")
    endif()
endif()

# Bootstrap vcpkg if its binary doesn't exist yet.
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/vcpkg/vcpkg" AND
   NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/vcpkg/vcpkg.exe")
    message(STATUS "Bootstrapping vcpkg...")
    if(WIN32)
        execute_process(
            COMMAND "${CMAKE_SOURCE_DIR}/third_party/vcpkg/bootstrap-vcpkg.bat" -disableMetrics
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/third_party/vcpkg"
            RESULT_VARIABLE bootstrap_result)
    else()
        execute_process(
            COMMAND "${CMAKE_SOURCE_DIR}/third_party/vcpkg/bootstrap-vcpkg.sh" -disableMetrics
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/third_party/vcpkg"
            RESULT_VARIABLE bootstrap_result)
    endif()
    if(NOT bootstrap_result EQUAL 0)
        message(FATAL_ERROR "vcpkg bootstrap failed")
    endif()
endif()

################################################################################
# project

set(TL_VERSION_MAJOR "2")
set(TL_VERSION_MINOR "0")
set(TL_VERSION_PATCH "0")
set(TL_VERSION_DEV "-dev")
set(TL_VERSION_FULL ${TL_VERSION_MAJOR}.${TL_VERSION_MINOR}.${TL_VERSION_PATCH}${TL_VERSION_DEV})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(
    tlRender2
    VERSION ${TL_VERSION_MAJOR}.${TL_VERSION_MINOR}.${TL_VERSION_PATCH}
    DESCRIPTION "tlRender is a library for rendering timelines"
    HOMEPAGE_URL "https://github.com/grizzlypeak3d/tlRender"
    LANGUAGES CXX C)

option(TL_BUILD_TESTS "Build the tlRender2 tests" OFF)

################################################################################
# dependencies

find_package(Threads)

set(ftk_API "GL_4_1" CACHE STRING "Graphics API (GL_4_1, GL_4_1_Debug, GLES_2)")
set(ftk_INSTALL OFF CACHE BOOL "" FORCE)
set(ftk_TESTS OFF CACHE BOOL "" FORCE)
set(ftk_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/feather-tk EXCLUDE_FROM_ALL)
# \todo Move these to feather-tk
if(NOT TARGET ftk::ftkCore)
    add_library(ftk::ftkCore ALIAS ftkCore)
endif()
if(NOT TARGET ftk::ftkUI)
    add_library(ftk::ftkUI ALIAS ftkUI)
endif()

set(OTIO_CXX_INSTALL ON CACHE BOOL "" FORCE)
set(OTIO_FIND_IMATH ON CACHE BOOL "" FORCE)
set(OTIO_SHARED_LIBS ${BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/otio)

################################################################################
# configuration

add_compile_definitions(TL_STATIC)

include(GNUInstallDirs)

################################################################################
# source

add_subdirectory(libs/Core)
add_subdirectory(libs/IO)
add_subdirectory(libs/InfoApp)
add_subdirectory(libs/PlayerApp)
add_subdirectory(libs/Render)
add_subdirectory(libs/RenderApp)
add_subdirectory(libs/Timeline)
add_subdirectory(libs/UI)

add_subdirectory(apps/tlinfo)
add_subdirectory(apps/tlrender)
add_subdirectory(apps/tlplayer)

################################################################################
# tests

if(TL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

################################################################################
# install / package config

include(CMakePackageConfigHelpers)
set(TL_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/tlRender2)
install(EXPORT tlRender2Targets
    FILE tlRender2Targets.cmake
    NAMESPACE tlRender2::
    DESTINATION ${TL_INSTALL_CMAKEDIR})
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/tlRender2ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/tlRender2Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/tlRender2Config.cmake"
    INSTALL_DESTINATION ${TL_INSTALL_CMAKEDIR})
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/tlRender2Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/tlRender2ConfigVersion.cmake"
    DESTINATION ${TL_INSTALL_CMAKEDIR})
