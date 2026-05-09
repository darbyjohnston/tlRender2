add_library(tlCore)

target_sources(tlCore
    PRIVATE
        include/tl/Core/Audio.h
        include/tl/Core/AudioInline.h
        include/tl/Core/Export.h
        include/tl/Core/Time.h
        include/tl/Core/TimeInline.h
        include/tl/Core/Util.h
        src/Audio.cpp
        src/Time.cpp)

target_include_directories(tlCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party/feather-tk/lib>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tlCore PUBLIC ftk::ftkCore)

target_compile_features(tlCore PUBLIC cxx_std_17)

target_compile_definitions(tlCore PUBLIC
    -DTL_VERSION_MAJOR=${TL_VERSION_MAJOR}
    -DTL_VERSION_MINOR=${TL_VERSION_MINOR}
    -DTL_VERSION_PATCH=${TL_VERSION_PATCH}
    -DTL_VERSION_DEV="${TL_VERSION_DEV}"
    -DTL_VERSION_FULL="${TL_VERSION_FULL}")

install(TARGETS tlCore
    EXPORT tlRender2Targets
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "._*" EXCLUDE)

