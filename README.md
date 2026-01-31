# CopyCleaner scripting language

See /docs/ for documentation

See [TODOS.md](TODOS.md) for development progress

## Building from Source

### Prerequisites
- CMake 3.10 or higher
- C++ compiler with C++20 support (Developed using GCC, but MSVC, CLANG, ... should work fine)
- Ninja (optional, but recommended)

### Compilation

Navigate to the cpp directory and build:

```bash
cd cpp
cmake -B build -S . -G Ninja
cmake --build build
```

Or using make:

```bash
cd cpp
cmake -B build -S .
cmake --build build
```

The compiled executable will be located in `cpp/build/`
