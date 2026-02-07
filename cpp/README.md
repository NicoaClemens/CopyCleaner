# CopyCleaner C++ Interpreter

Tree-walking interpreter for the CopyCleaner scripting language (.ccl files).

## Architecture Overview

```
Source (.ccl) -> Lexer -> Parser -> AST -> Interpreter -> Execution
```

### Pipeline

1. **Lexer** ([lexer.h](include/lexer.h), [lexer.cpp](src/lexer.cpp))
   - Tokenizes source code into a stream of tokens
   - Handles identifiers, literals (int, float, string, f-string, regex, bool), keywords, operators
   - Skips whitespace and comments

2. **Parser** ([parser.h](include/parser.h), [parser.cpp](src/parser.cpp))
   - Constructs Abstract Syntax Tree (AST) from token stream
   - Uses recursive descent with precedence climbing for expressions
   - Produces `Statement` and `Expr` nodes

3. **AST** ([ast.h](include/ast.h), [ast.cpp](src/ast.cpp))
   - Defines syntax tree node types:
     - **Expressions**: Literals, Variables, BinaryOp, UnaryOp, Call, FunctionCall, Index, Ternary
     - **Statements**: Assignment, VarDecl, If, While, FunctionDef, Return, Break, Continue, ExprStmt
   - Type system: `AstType` (Int, Float, Bool, String, Regex, Match, List, Null)

4. **Runtime** ([runtime.h](include/runtime.h), [runtime.cpp](src/runtime.cpp))
   - **Interpreter**: Tree-walking evaluator that executes AST nodes
   - **Environment**: Lexical scoping with parent chain for variable resolution
   - **ExecFlow**: Control flow handling (Return, Break, Continue, Exit)
   - Evaluates expressions recursively, executes statements sequentially

### Data Model

**RuntimeValue** ([runtime_value.h](include/runtime_value.h))
- Dynamic type system using `std::variant`
- Types: `Int`, `Float`, `Bool`, `String`, `List`, `Match`, `Regex`, `Null`
- Runtime type checking during operations

**Type System**
- Static type annotations in source code (`AstType`)
- Runtime type enforcement via `RuntimeValue`
- Type errors reported with source location

### Error Handling

**Result<T>** ([result.hpp](include/result.hpp))
- Monadic error handling pattern
- Wraps `Ok<T>` or `Err<Error>`
- Errors propagate through pipeline with source spans

**Error** ([errors.hpp](include/errors.hpp))
- Categories: Runtime, Syntax, Type, Arity, DivideByZero, Exit, Parse
- Includes optional source location (`Span`) for diagnostics

### Built-in System

**Builtin Modules** ([builtins/](include/builtins/))
- `Clipboard`: System clipboard read/write
- `Console`: Console out interface
- `Logger`: Debug logging with levels
- `Alert`: Platform-specific message boxes

**Builtin Functions** ([utils/builtin_functions.h](include/utils/builtin_functions.h))
- Dispatcher: `call_builtin()` routes function names to implementations
- Functions: `print`, `input`, `clipboard_read`, `clipboard_write`, `log`, `alert`, `exit`, `int`, `float`, `bool`, `str`, `len`, `type`

**Method Dispatch** ([utils/method_dispatcher.hpp](include/utils/method_dispatcher.hpp))
- Dynamic method resolution for built-in types
- String methods: `split`, `replace`, `trim`, `upper`, `lower`, `contains`, `startsWith`, `endsWith`, etc.
- Regex methods: `match`, `matchAll`, `getAll`, `replace`
- List methods: `get`, `set`, `push`, `pop`, `shift`, `length`, `join`

## Key Components

### Lexer Implementation
- Character-by-character scanning with lookahead
- Escape sequence handling in strings
- Regex literal parsing with flags (`/pattern/flags`)
- F-string interpolation lexing (`f"text {expr}"`)

### Parser Strategy
- **Recursive descent** for statements
- **Precedence climbing** for expressions:
  ```
  Ternary (? :) -> LogicalOr (||) -> LogicalAnd (&&) -> 
  Comparison (==, !=, <, >, <=, >=) -> Addition (+, -, ++) -> 
  Multiplication (*, /) -> Exponentiation (**) -> 
  Unary (!, -) -> Postfix (., [], ()) -> Primary
  ```
- Type annotations parsed and stored in AST

### Interpreter Execution
- **Expression evaluation**: Returns `Result<RuntimeValue>`
  - Literals: Direct value construction
  - Variables: Environment lookup
  - Binary/Unary ops: Type-checked arithmetic/logic
  - Calls: Method dispatch or function invocation
  - Function calls: Environment capture, parameter binding
  - Index: List element access
  - Ternary: Conditional short-circuit evaluation

- **Statement execution**: Returns `Result<ExecFlow>`
  - Assignments: Variable mutation in environment
  - VarDecl: New binding with type checking
  - If/While: Control flow with nested scopes
  - FunctionDef: Closure capture in function registry
  - Return/Break/Continue: Control flow signaling
  - ExprStmt: Expression evaluation for side effects

### Environment Scoping
```cpp
struct Environment {
    std::unordered_map<std::string, RuntimeValue> variables;
    env_ptr parent;  // nullptr for global scope
    
    std::optional<RuntimeValue> get(name);  // Traverses parent chain
    void set(name, value);                   // Sets in current scope
}
```
- Global environment created at interpreter startup
- Function calls create nested environments with parent link
- Block scopes (if/while) use nested environments

### Control Flow
```cpp
struct ExecFlow {
    std::variant<None, Return, Break, Continue, Exit> value;
}
```
- Propagates through statement execution
- Breaks out of loops/functions when non-`None`
- `Return` carries the return value

## Build System

**CMake** ([CMakeLists.txt](CMakeLists.txt))
- C++20 required
- External dependencies: None 
- Build outputs: `copycleaner` executable

**Entry Point** ([src/main.cpp](src/main.cpp))
1. Read script file from command-line argument
2. Lex -> Parse -> Execute
3. Error reporting with exit codes:
   - `1`: File I/O error
   - `2`: Parse error
   - `3`: Runtime error
   - `0`: Success or graceful `exit()` call

## Execution Flow Example

```cpp
Source: x() = 5 + 3;

1. Lexer produces:
   [Identifier("x"), LParen, RParen, Assign, Int(5), Plus, Int(3), Semicolon]

2. Parser builds AST:
   VarDecl {
     name: "x",
     type: null (inferred),
     init: BinaryOp { op: Add, left: Literal(5), right: Literal(3) }
   }

3. Interpreter executes:
   - Evaluate BinaryOp(Add, 5, 3) -> RuntimeValue::Int(8)
   - Bind "x" in Environment -> variables["x"] = RuntimeValue::Int(8)
   - Return ExecFlow::None
```

## Type System Details

**Static vs Dynamic**
- Type annotations are **optional** in source code
- Type inference from initializer expressions
- Runtime type checking enforced during operations
- Type mismatches produce `ErrorKind::Type` errors

**Type Coercion**
- Explicit: `int()`, `float()`, `bool()`, `str()` builtins

**Callable Types**
- **Functions**: User-defined via `function` keyword
  - Stored in `Interpreter::functions` registry
  - Closure: Captures environment at definition
- **Methods**: Built-in operations on values
  - Dispatched via `method_dispatcher`

## Memory Management

- **AST**: `std::unique_ptr` for recursive structures
- **Environment**: `std::shared_ptr` for shared parent chains
- **Runtime values**: Stack-allocated `std::variant` with value semantics
- **Strings/Lists**: `std::string`/`std::vector` handle heap allocation

## Extension Points

**Adding Built-in Functions**
1. Implement in [utils/builtin_functions.cpp](src/utils/builtin_functions.cpp)
2. Add case to `call_builtin()` dispatcher

**Adding Methods**
1. Implement in appropriate `*_methods.cpp` (string/list/regex)
2. Register in `method_dispatcher.cpp`

**Adding Types**
1. Extend `AstType` and `RuntimeValue` variants
2. Add type checking in interpreter operations
3. Implement type-specific methods
