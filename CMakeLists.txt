cmake_minimum_required(VERSION 3.31)

include(FetchContent)

project(flora-engine C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "ASAN" AND NOT UNIX)
  message(FATAL_ERROR "ASAN builds are only allowed on UNIX-like systems")
endif()

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "ASAN")
    add_compile_options(-g3 -O0 -Wall -Wextra)

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fstack-protector-strong")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wformat=2 -Wformat-security")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wnull-dereference -Wstack-protector")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Warray-bounds -Wshift-overflow")
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "ASAN")
    if (UNIX)
      set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
      set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=undefined")
    endif ()
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -fno-delete-null-pointer-checks -flto=auto)
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Perf")
    add_compile_options(-O2 -g -fno-omit-frame-pointer -fno-delete-null-pointer-checks -flto=auto)
  endif()
elseif (MSVC)
  # MSVC-compatible warnings/optimizations; keep LTO off by default to avoid toolchain issues
  add_compile_options(/W4)
endif()


# On Windows (MinGW/MSVC) system freetype/harfbuzz are usually not available.
# Use SDL_ttf's vendored copies so the project builds out-of-the-box.
if(WIN32)
  set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored freetype/harfbuzz on Windows" FORCE)
endif()

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

# On Windows the SDL shared libraries need to be next to the exe at runtime.
# Copy them next to flora_engine_demo after building so `build/flora_engine_demo.exe` runs without manual PATH setup.
if(WIN32)
  add_custom_command(TARGET flora_engine_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:flora_engine_demo>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
            $<TARGET_FILE_DIR:flora_engine_demo>
    COMMENT "Copying SDL3 DLLs next to flora_engine_demo"
    VERBATIM
  )
endif()
