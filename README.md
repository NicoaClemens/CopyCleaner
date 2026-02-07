# CopyCleaner scripting language

**!! Under Development. No release yet!**

See /docs/ for documentation

Codebase under /cpp/

Script examples in /scripts/

Vs code extension for script highlighting in /vscode-extension/

See [TODOS.md](TODOS.md) for development progress

## Contribute

You are welcome to
- create your own fork
- open issues for bugs or feature requests
- submit pull requests for fixes or improvements

By contributing you agree that your code will be published under the [BSD-3 Clause License](LICENSE)

Please adhere to the [project style guide](STYLE.md) and the `cpp/.clang-format`. Write clear, human-readable and maintainable code. AI-generated code is **not accepted**

## Code examples

All these examples work in the current build. See `/scripts` for more examples

**Removes all non-ascii chars from the clipboard**
```cpp
// Read current clipboard content
string clipboard_content() = clipboard_read();

// Define regex pattern to match non-ASCII characters
regex non_ascii_pattern(/[^\x00-\x7F]/g);

// Find all non-ASCII characters
list<match> non_ascii_chars() = non_ascii_pattern.getAll(clipboard_content);

int num_found() = non_ascii_chars.length();

// If non-ASCII characters were found, remove them
if (num_found > 0) {
    // Replace all non-ASCII characters with empty string
    string cleaned() = clipboard_content;
    
    int i(0);
    while (i < num_found) {
        match m() = non_ascii_chars.get(i);
        cleaned = cleaned.replaceMatch(m, "");
        i = i + 1;
    };
    
    // Write cleaned text back to clipboard
    clipboard_write(cleaned);
    
} else {
    showAlert("No non-ASCII characters found in clipboard.");
};
```

## Building from Source

### Prerequisites
- CMake 3.10 or higher
- C++ compiler with C++20 support (Developed using GCC, but MSVC, CLANG, ... should work fine)
- Ninja (optional, but recommended)

Standard building using `CMakeLists.txt` in `/cpp/`