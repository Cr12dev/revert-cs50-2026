cmake_minimum_required(VERSION 3.10)
project(SDLImageRenderer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Configuración de warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Buscar SDL2
find_package(SDL2 REQUIRED)

if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 no encontrado!")
endif()

# Archivos fuente
set(SOURCES
    src/main.c
    src/graphics.c
)

# Ejecutable principal
add_executable(sdl_image_renderer ${SOURCES})

# Incluir directorios
target_include_directories(sdl_image_renderer PRIVATE src)

# Linkear bibliotecas
target_link_libraries(sdl_image_renderer ${SDL2_LIBRARIES})

# Configuración específica para macOS
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(sdl_image_renderer ${COCOA_LIBRARY})
endif()

# Crear directorio de assets si no existe
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/images)

# Script para crear imagen de prueba (opcional)
add_custom_command(
    TARGET sdl_image_renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Coloca tus archivos BMP en assets/images/"
)
