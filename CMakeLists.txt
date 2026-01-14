cmake_minimum_required(VERSION 3.10)
project(SDLImageRenderer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

if(NOT SDL2_FOUND OR NOT SDL2_IMAGE_FOUND OR NOT SDL2_TTF_FOUND)
    
    
    set(SDL2_LIBRARIES ${SDL2_LINK_LIBRARIES})
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
    set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LINK_LIBRARIES})
    set(SDL2_TTF_LIBRARIES ${SDL2_TTF_LINK_LIBRARIES})
endif()

set(SOURCES
    src/main.c
    src/graphics.c
    src/ui.c
)

add_executable(sdl_image_renderer ${SOURCES})

target_include_directories(sdl_image_renderer PRIVATE 
    src 
    ${SDL2_INCLUDE_DIRS} 
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(sdl_image_renderer ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})

if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(sdl_image_renderer ${COCOA_LIBRARY})
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/images)

add_custom_command(
    TARGET sdl_image_renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Coloca tus archivos BMP en assets/images/"
)
