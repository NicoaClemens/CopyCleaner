# Copycleaner Scripting Language Overview

**Purpose:** A simple, interpreted scripting language for customizable clipboard manipulation, optional alerts, logging, and console output.

---

## Syntax

- C++-inspired syntax with minor differences.
- **No OOP**, **all code in a single file**, no includes.
- **Semicolons required** everywhere (including after `{}` blocks). Last semicolon in a file is optional.
- **Whitespace ignored**: line breaks, tabs, repeated spaces removed during parsing.
- **Strings**: Line breaks allowed. A `\` before a line break continues the string without adding a newline.
- **Top-down parsing**: Functions must be defined before use. No `main()` entry point.
- **Comments**: `//` starts a single-line comment.

---

## Variables & Types

- **Typed variables**; local scopes exist for `if`/`elif`/`else`, loops, and functions. Variables outside these scopes are global.
- **Implicit conversions**:
  - `int ↔ float`
  - Any type can be converted to `string`
  - Explicit conversion syntax: `newType myVar(oldVar);`
- empty initialiser defaults to null
- strings are encapsulated by " ", regexes by \ \, lists by { }
- Variable assignment using = allowed, but initialization must be via `int n(4)` or `int n() = {expression}`
  
| Type | Example | Notes / Members / Methods |
| ---- | ------- | ------------------------- |
| `string` | `string s("Hello")` | Multi-line strings are allowed but line breaks will appear in the finished string. Use `\` just before the line break to prevent the line break to be added into the string. |
| `int` | `int n(-4)` | - |
| `float` | `float f(1.5)` | - |
| `boolean` | `boolean b(true)` | - |
| `regex` | `regex r(\^[a-zA-Z0-9_]\, "rm")` | Methods: `.getAll(string)` → `list<match>`; Members: `.re`, `.flags` |
| `list<T>` | `list<int> l({1,2,3})` | Methods: `.get(index)`. Negative index returns item at index counted backwards from end of list. |
| `match` | N/A | Returned by regex `.get()`, Members: `.start`, `.end`, `.content`. |

---

## Functions

```cpp
function name(type1 arg1, type2 arg2, ...) { ... };
function name returns type(type1 arg1, ...) { ...; return value; };

```

 - Typed arguments and return values.
 - NO overloading or default arguments.
 - Variables must be declared before assignment
 - Example:

```cpp

function add(int i, int j) returns int { return(i + j); };

int number(4);
number = add(4, 1);                 // 5 
int number2(0) = add(number, 2)     // 7

```

 - Recursion allowed; no automatic prevention of infinite recursion.

---

## Format strings (fstring)

- The language supports a simple positional format-string helper called `fstring`.
- Usage example: `string myString(f"%2, %1, %3", "a", 10, true);` evaluates to `"10, a, true"`.
- The first argument is a format template where `%n` inserts the nth following argument (1-based).


## Operators

 - Arithmetic: `** * / + -` 
 - Comparison: `== > < >= <= !=`
 - Logical: `! && ||`
 - Ternary: `condition ? value : default`
 - Unary: `- !` 
 - String concatenation: `++` 
 - No operator overloading.
 - OOE: Arithmetic -> Comparison -> Logical -> Ternary. Inside those categories as listed (`**` before `*` before `/` ...)

### Notes
 - dividing int by int truncates to int -> if float result is desired, cast one value to float beforehand

## Control Flow

 - if statements:

 ```cpp
 if (condition) { ... }; elif (condition) { ... }; else { ... };
 ```
 - loops:

 ```cpp
 while (condition) { ... if (cond) { break; }; if (cond2) { continue; }; };
 ```
 - local scopes created within if, elif, else and loop blocks.
 - no for-loops.
---

## Clipboard, Alerts, and Logging

 - Clipboard read/wrire accessible via built-in methods.
 - Optional pop-up alerts.
 - Logging to file or console supported

## Error Handling

 - No recovery. Any syntax or runtime error terminates the script immediately

---

## Notes

 - Everything must be in a single file; no includes or external modules.
 - All pre-defined functions are to be documented separately.
 - setEncoding() method must be called at top of file before logging (file, console), alerts or clipboard reading/writing is ever called
