cmake_minimum_required(VERSION 4.2)
project(gdtk
        LANGUAGES C
        VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES FALSE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_executable(${CMAKE_PROJECT_NAME}_sandbox)
target_sources(${CMAKE_PROJECT_NAME}_sandbox PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/game.c)
target_link_libraries(${CMAKE_PROJECT_NAME}_sandbox PRIVATE
        ${CMAKE_PROJECT_NAME})

# Set to log Info or higher priority
add_compile_definitions(GDTK_LOG_PRIORITY=4)

add_library(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        ${SOURCE_FILES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)


# vendor libraries

# SDL
# https://github.com/libsdl-org/SDL.git

set(SDL_STATIC ON)
set(SDL_SHARED OFF CACHE INTERNAL "")
set(SDL_TESTS OFF CACHE INTERNAL "")
set(SDL_EXAMPLES OFF CACHE INTERNAL "")

add_subdirectory(vendor/sdl)
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        SDL_MAIN_USE_CALLBACKS)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
        SDL3::SDL3)
