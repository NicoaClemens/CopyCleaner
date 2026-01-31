# CopyCleaner C++ Style Guide

C++ Standard
- Target: C++20

Header file naming
- Use `.h` for plain declarations and headers that contain only small inline functions (e.g., simple getters/setters).
- Use `.hpp` for headers that include implementations longer than one line, templates, or multiple inline function bodies.

Top of file (not strict)
- first line: `//` followed by a whitespace followed by the file name 
- for .h: 
  - second line: `// Declares: ` followed by key class/struct/non-class function names
- for .hpp:
  - second line: `// Declares/Implements: ` followed by key class/struct/non-class function names
- for .cpp:
  - second line: `// Implements <header file>` 
After that, one free line, then includes

- Order includes in each file:
  1. (cpp only) Corresponding header file
  2. Standard library headers (alphabetically sorted)
  3. Third-party/library headers (alphabetically sorted)
  4. Project headers (alphabetically sorted)
- One blank line between each group

Header contents
- Use `#pragma once` in all headers.

Formatting rules
- Indentation: 4 spaces, never tabs.
- Brace style: Attach (K&R / same line opening brace).
- Pointer binding: `int* p;` (left binding)
- Reference binding: `int& p;` (left binding)

Small conventions
- Filename case: lower_snake_case for files and directories.
- Use `camelCase` for local variables, `snake_case` for file names.

Documentation

- Use doxygen-style docstrings exclusively (/// @brief, /// @param, etc)
- Docstrings should be added for all public APIs
- document non-obvious behaviour, side effects, preconditions
- don't add docstrings for private methods, self-explanatory methods like getter/setters or methods where the name and signature explain behaviour
- only add docstrings at declaration, not again for implementation
