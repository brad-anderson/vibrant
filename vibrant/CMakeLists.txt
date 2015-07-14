cmake_minimum_required(VERSION 3.3)


add_library(vibrant

    include/vibrant/vibrant.hpp
    include/vibrant/body.hpp
    include/vibrant/color.hpp
    include/vibrant/ease.hpp
    include/vibrant/renderable.hpp
    include/vibrant/vector.hpp
    source/color.cpp
    source/ease.cpp
    source/mouse.cpp
)


find_package(Boost 1.57 REQUIRED)
add_subdirectory(../external/entityx external/entityx)

target_include_directories(vibrant
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ../external/entityx
    PUBLIC include
    #PRIVATE src
)

target_link_libraries(vibrant
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC entityx
)
