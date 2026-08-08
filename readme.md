# Flora Engine

Flora Engine is a 2D UI engine written in C, built on SDL3.

## Project Layout

- `flora_engine_lib` — the engine itself, built as a static library (output: `flora`)
- `flora_engine_demo` — an example application (`examples/basic_demo.c`) linked against the library

## Dependencies

All dependencies are fetched and built automatically by CMake — you don't need to install SDL3, SDL3_ttf, or SDL3_image yourself.

You do need:

- CMake 3.31+
- Git (used by CMake to fetch dependencies)
- A C compiler — MinGW on Windows, gcc/clang on Linux/macOS

## Building

### Windows
```sh
    cmake -B build -G "MinGW Makefiles"
    cmake --build build
```

### Linux

```sh
cmake -B build
cmake --build build
```

The first build will take longer than usual while SDL3, SDL3_ttf, and SDL3_image are cloned and compiled.

## Running the demo

After building, run the `flora_engine_demo` executable produced under `build/`.

## Capabilities

Flora currently supports:

- Box and text widgets, with FIT / FIXED / GROW sizing along both axes
- Two layout directions: left-to-right and top-to-bottom
- Multiple windows, each with their own set of named screens
- Event dispatch to widgets (e.g. mouse-down)
- Font loading and text rendering

This image shows a demo screen rendered using flora engine:

![flora-demo.png](assets/flora-demo.png)

Flora is still in active development — see `TODO.md` for planned work.

## License

MIT — see `LICENSE`.

## Code Style

Function naming convention: `verb_object()` — e.g. `destroy_table()`.
