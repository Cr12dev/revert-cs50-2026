cmake_minimum_required(VERSION 3.10)
project(HolaMundoSDL LANGUAGES C)

# Configuración del compilador
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Opciones de compilación
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()

# Buscar SDL2
find_package(SDL2 REQUIRED)

# Verificar si SDL2 fue encontrado
if(SDL2_FOUND)
    message(STATUS "SDL2 encontrado: ${SDL2_VERSION}")
    include_directories(${SDL2_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SDL2 no encontrado!")
endif()

# Crear ejecutable
add_executable(main src/main.c)

# Linkear con SDL2
target_link_libraries(revert ${SDL2_LIBRARIES})

# En macOS, agregar frameworks necesarios
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(holamundo ${COCOA_LIBRARY})
endif()

# En Windows, configuraciones adicionales
if(WIN32)
    # Agregar archivos DLL si es necesario
    if(EXISTS "${SDL2_DIR}/bin/SDL2.dll")
        configure_file(
            "${SDL2_DIR}/bin/SDL2.dll"
            "${CMAKE_CURRENT_BINARY_DIR}/SDL2.dll"
            COPYONLY
        )
    endif()
endif()
