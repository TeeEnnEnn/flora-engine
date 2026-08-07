cmake_minimum_required(VERSION 3.31)

include(FetchContent)

project(flora-engine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FetchContent_Declare (
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG 147a8ee32dbf9ac02f3794964490687b6bbda1bc # Release 3.4.14
        GIT_SHALLOW TRUE
)

FetchContent_Declare (
        SDL3_TTF
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG a1ce3670aec736ecbf0936c43f2f0cc53aa61e5b # Release 3.2.2
        GIT_SHALLOW TRUE
)

FetchContent_Declare (
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG bec9134a26c7d0f31b36d6083c25296e04cabff5 # Release 3.4.4
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(
        SDL3 SDL3_TTF SDL3_image
)

set(LIB_SOURCES
        "src/flora_events.c"
        "src/flora_fonts.c"
        "src/flora_main.c"
        "src/flora_screens.c"
        "src/flora_widgets.c"
        "src/table.c"
)

add_library(flora_engine_lib STATIC ${LIB_SOURCES})
set_target_properties(flora_engine_lib PROPERTIES OUTPUT_NAME flora)

target_include_directories(flora_engine_lib PUBLIC include)
target_include_directories(flora_engine_lib PRIVATE "src/headers")

target_link_libraries(flora_engine_lib PUBLIC SDL3::SDL3 SDL3_ttf::SDL3_ttf)

add_executable(flora_engine_demo WIN32 examples/basic_demo.c)
target_link_libraries(flora_engine_demo PRIVATE flora_engine_lib)
