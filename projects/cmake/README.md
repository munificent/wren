# Wren CMake

Allows building the [Wren scripting language](https://wren.io) with [CMake](https://cmake.org).

## Usage

In your project, add Wren's `cmake` directory, and then target either `wren` or `wren_shared`...

``` cmake
add_subdirectory(path/to/wren/projects/cmake)
target_link_libraries(myproject PUBLIC wren)
```

## Options

You can enable or disable a few options with CMake to customize the build...

Option | Description | Default
--- | --- | :-:
`WREN_META` | Enables the Meta Wren module | `ON`
`WREN_RANDOM` | Enables the Random Wren module | `ON`
`WREN_BUILD_STATIC` | Build Wren as a static library | `ON`
`WREN_BUILD_SHARED` | Build Wren as a shared library | `OFF`
`WREN_BUILD_TEST` | Build Wren's testing framework | `OFF`

## Testing

To build Wren locally, and test the CMake build, use the following:

``` sh
cd projects/cmake
mkdir build
cd build
cmake .. -DWREN_BUILD_TEST=ON
make
make test
```
