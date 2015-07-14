cmake_minimum_required(VERSION 3.3)
cmake_policy(SET CMP0015 NEW)

add_library(vibrant-cairo

    include/vibrant/cairo/render.hpp
    source/render.cpp
)


if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CAIRO_ARCH "x64" )
    else(CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(CAIRO_ARCH "x86" )
    endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    find_path(Cairo_INCLUDE_DIRS cairo/cairo.h HINTS "../external/win/cairo-${CAIRO_ARCH}/include")
    find_library(Cairo_LIBRARIES NAMES cairo HINTS "../external/win/cairo-${CAIRO_ARCH}/lib")
    find_path(Cairo_BIN_DIRS libcairo-2.dll HINTS "../external/win/cairo-${CAIRO_ARCH}/bin")
else()
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(Cairo REQUIRED cairo)
endif()

target_include_directories(vibrant-cairo
    PUBLIC ${Cairo_INCLUDE_DIRS}
    PUBLIC ../external/entityx
    PUBLIC include
)

target_link_libraries(vibrant-cairo
    PUBLIC vibrant
    PUBLIC ${Cairo_LIBRARIES}
)

