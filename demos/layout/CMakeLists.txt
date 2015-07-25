cmake_minimum_required(VERSION 3.3)

add_executable(vibrant-demo-layout WIN32

    source/layout.cpp
)

find_package(wxWidgets 3.0.0 COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

target_link_libraries(vibrant-demo-layout
    PRIVATE vibrant
    PRIVATE vibrant-cairo
    PRIVATE ${wxWidgets_LIBRARIES}
)

if(WIN32)
    # Cairo_BIN_DIRS defined in vibrant-cairo
    file(COPY ${Cairo_BIN_DIRS}/libcairo-2.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR} NO_SOURCE_PERMISSIONS)
    file(COPY ${Cairo_BIN_DIRS}/libfontconfig-1.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR} NO_SOURCE_PERMISSIONS)
    file(COPY ${Cairo_BIN_DIRS}/libexpat-1.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR} NO_SOURCE_PERMISSIONS)
    file(COPY ${Cairo_BIN_DIRS}/libfreetype-6.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR} NO_SOURCE_PERMISSIONS)
    file(COPY ${Cairo_BIN_DIRS}/libpng14-14.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR} NO_SOURCE_PERMISSIONS)
    file(COPY ${Cairo_BIN_DIRS}/zlib1.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR} NO_SOURCE_PERMISSIONS)
endif()
