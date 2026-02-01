# CopyCleaner Language Support for VS Code

Provides syntax highlighting and language support for CopyCleaner scripting language (`.ccl` files).

## Features

- **Syntax Highlighting**: Full syntax highlighting for CopyCleaner language
  - Keywords: `if`, `elif`, `else`, `while`, `break`, `continue`, `return`, `function`, `returns`
  - Types: `int`, `float`, `boolean`, `string`, `regex`, `match`, `list`
  - Built-in functions: `exit`, `fstring`, `setLog`, `log`, `print`, etc.
  - Operators: arithmetic, comparison, logical, ternary
  - Comments: single-line `//` comments
  - Literals: strings, numbers, booleans

- **Language Configuration**:
  - Auto-closing brackets and quotes
  - Comment toggling with `Ctrl+/`
  - Bracket matching
  - Code folding

## Installation

### From Source (Development)

1. Clone the repository
2. Copy the `vscode-extension` folder to your VS Code extensions directory:
   - **Windows**: `%USERPROFILE%\.vscode\extensions\copycleaner-language-0.1.0`
   - **macOS/Linux**: `~/.vscode/extensions/copycleaner-language-0.1.0`
3. Reload VS Code

### Testing Locally

1. Open the `vscode-extension` folder in VS Code
2. Press `F5` to launch a new Extension Development Host window
3. Open a `.ccl` file to see syntax highlighting

## File Association

This extension automatically activates for files with the `.ccl` extension.

## Language Overview

CopyCleaner is a simple, interpreted scripting language for clipboard manipulation, logging, and console output with C++-inspired syntax.

### Example

```copycleaner
// Set up logging
boolean logSetup(false);
logSetup = setLog("script.log");

// Function definition
function factorial returns int(int n) {
    if (n <= 1) {
        return 1;
    };
    int prev(0);
    prev = n - 1;
    return n * factorial(prev);
};

// Main code
int result(0);
result = factorial(5);

string msg("");
msg = fstring("Factorial of 5 is %1", result);
msg = print(msg);
msg = log(msg);
```

## Syntax Highlights

- **Control Flow**: `if`, `elif`, `else`, `while`, `break`, `continue`, `return`
- **Data Types**: `int`, `float`, `boolean`, `string`, `list`, `regex`, `match`
- **Operators**: `+`, `-`, `*`, `/`, `**` (power), `++` (concat), `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`, `!`, `? :`
- **Comments**: `// single-line comments`

## Contributing

Issues and contributions welcome at the [CopyCleaner repository](https://github.com/NicoaClemens/CopyCleaner).

## License

MIT
