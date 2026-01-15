cmake_minimum_required(VERSION 3.10)
project(SDLImageRenderer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Intentar encontrar paquetes usando find_package primero (recomendado para Windows/vcpkg)
find_package(SDL2 CONFIG QUIET)
find_package(SDL2_image CONFIG QUIET)
find_package(SDL2_ttf CONFIG QUIET)

if(NOT SDL2_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
    set(SDL2_LIBRARIES ${SDL2_LINK_LIBRARIES} ${SDL2_IMAGE_LINK_LIBRARIES} ${SDL2_TTF_LINK_LIBRARIES})
else()
    # Si se encontró via CONFIG (Windows/vcpkg), los targets suelen estar disponibles
    set(SDL2_INCLUDE_DIRS "") # No se necesitan si usamos targets
    set(SDL2_LIBRARIES SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)
endif()

set(SOURCES
    src/main.c
    src/graphics.c
    src/ui.c
    src/plugin_manager.c
)

add_executable(sdl_image_renderer ${SOURCES})

target_include_directories(sdl_image_renderer PRIVATE src include ${SDL2_INCLUDE_DIRS})
target_link_libraries(sdl_image_renderer ${SDL2_LIBRARIES} m ${CMAKE_DL_LIBS})

# Plugins
add_library(negative_plugin SHARED plugins/negative_plugin.c)
target_include_directories(negative_plugin PRIVATE src include ${SDL2_INCLUDE_DIRS})
target_link_libraries(negative_plugin ${SDL2_LIBRARIES})
set_target_properties(negative_plugin PROPERTIES PREFIX "" SUFFIX ".so")


if(WIN32 AND MSVC)
    set_target_properties(sdl_image_renderer PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(sdl_image_renderer ${COCOA_LIBRARY})
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/images)
