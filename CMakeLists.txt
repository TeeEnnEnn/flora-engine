cmake_minimum_required(VERSION 3.31)

include(FetchContent)

set(CMAKE_C_COMPILER gcc)
project(flora-engine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FetchContent_Declare (
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG 147a8ee32dbf9ac02f3794964490687b6bbda1bc # Release 3.4.14
)

FetchContent_Declare (
        SDL3_TTF
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG a1ce3670aec736ecbf0936c43f2f0cc53aa61e5b # Release 3.2.2
)

set(SOURCES
        "src/flora_events.c"
        "src/flora_fonts.c"
        "src/flora_screens.c"
        "src/flora_widgets.c"
        "src/flora_apps.c"
        "src/main.c"
        "src/table.c"
)

FetchContent_MakeAvailable(
        SDL3 SDL3_TTF
)

add_executable(flora-engine ${SOURCES})
target_include_directories(flora-engine PRIVATE "src/headers")

target_link_libraries(flora-engine PRIVATE SDL3::SDL3)
target_link_libraries(flora-engine PRIVATE SDL3_ttf::SDL3_ttf)
