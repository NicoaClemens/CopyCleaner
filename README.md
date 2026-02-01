# CopyCleaner scripting language

See /docs/ for documentation

Codebase under /cpp/

Script examples in /scripts/

Vs code extension for script highlighting in /vscode-extension/

See [TODOS.md](TODOS.md) for development progress

## Code examples

**Removes all non-ascii chars from the clipboard**
```cpp
// Read current clipboard content
string clipboard_content() = clipboard_read();

// Check if clipboard has text
boolean has_text() = clipboard_isText();

if (!has_text) {
    print("Clipboard does not contain text. Exiting.");
    exit();
};

// Define regex pattern to match non-ASCII characters
// ASCII characters are in range 0-127, so we match anything outside that range
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
    
    // Show confirmation
    string msg() = "Removed" ++ num_found ++ "non-ASCII character(s) from clipboard.";
    showAlertOK("Clipboard Cleaned", msg);
    
    print(msg);
}
else {
    showAlert("No non-ASCII characters found in clipboard.");
};
```

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
