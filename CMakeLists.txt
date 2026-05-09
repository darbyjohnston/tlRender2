cmake_minimum_required(VERSION 3.24)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0114 NEW)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

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

find_package(Threads)

include(GNUInstallDirs)

add_subdirectory(libs/Core)
add_subdirectory(libs/IO)
add_subdirectory(libs/Timeline)
add_subdirectory(libs/UI)

add_subdirectory(apps/tlrender)
add_subdirectory(apps/tlplayer)

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
