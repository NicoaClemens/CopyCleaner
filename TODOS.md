# TODOS

note: finished todos be removed in most cases, not checked off.

## most important

- [ ] Variable declarations not implemented in parser/interpreter (docs say `int n(4);` but parser only handles assignment)
- [ ] List literals and syntax not implemented (docs show `{1,2,3}` but parser has no support)
- [ ] Type conversion/casting not implemented (docs describe `newType myVar(oldVar);`)
- [ ] Many builtin methods not implemented (clipboard, logger, alert, regex methods, list methods)

## dev-related

- [ ] set up and document CMAKE -> c++20
- [ ] plan helper tool that allows to easily call scripts on shortcuts or per taskbar or whatever

## general todos

- [ ] lexer.cpp uses crude checks for unterminated strings/regexes (acknowledged in comment line 295)

## smaller todos


## new classes

- [ ] parser
- [ ] main entry point

## general behaviour

- [ ] exit() calls std::exit(0) internally -> change to graceful exit
- [ ] builtin methods (docs/methods.md) -> docs need to be fully fleshed out for ALL builtin methods, builtins must be implemented -> seperate /builtins class and/or folder

## code quality issues

- [ ] incomplete documentation sections in docs/methods.md: "## Logger" and "## Alert" have no content

## Style Guide Inconsistencies

### Documentation Issues (Public APIs only)
- [ ] result.hpp: Missing docstrings for Result class methods (is_ok, is_err, value, error) - public API with non-obvious behavior
- [ ] types_utils.hpp: Missing docstrings for operator==, to_string, is_truthy, matches_type functions - public APIs with specific behavior
- [ ] runtime.h: Missing docstrings for Interpreter public methods (run, eval_statements, eval_expr) - public API
- [ ] runtime.h: Missing docstrings for Environment public methods (get, set) - public API with non-obvious behavior (parent lookup)

### Formatting Issues
- [ ] runtime_utils.cpp line 10: C-style cast `(double)` should be `static_cast<double>`
- [ ] runtime_utils.cpp line 100: C-style cast `(double)a` should be `static_cast<double>`
- [ ] runtime_utils.cpp line 100: C-style cast `(double)b` should be `static_cast<double>`
- [ ] types_utils.hpp line 29-31: C-style casts should use static_cast

## Documentation Inconsistencies

### Syntax Documentation Issues
- [ ] docs/syntax.md describes variable declaration syntax `int n(4);` but parser.cpp only implements assignment
- [ ] docs/syntax.md describes list literals `{1,2,3}` but parser.cpp has no list literal parsing
- [ ] docs/syntax.md describes explicit conversion `newType myVar(oldVar);` but not implemented
- [ ] docs/syntax.md describes regex methods `.getAll(string)` but not implemented
- [ ] docs/syntax.md describes list methods `.get(index)` but not implemented
- [ ] docs/syntax.md describes fstring syntax but parser extracts 'f' prefix incorrectly (prepends to lexeme after string is tokenized)
- [ ] docs/syntax.md says "Last semicolon in a file is optional" but parser always expects semicolons
- [ ] docs/syntax.md describes empty initializer defaults to null `int n();` but not implemented

### Methods Documentation Issues
- [ ] docs/methods.md: Incomplete documentation for clipboard_write - shows `## function` instead of `###`
- [ ] docs/methods.md: Missing "## Logger" implementation details
- [ ] docs/methods.md: Missing "## Alert" implementation details
- [ ] docs/methods.md: setEncoding, clipboard_isText, clipboard_read, clipboard_write - none implemented
- [ ] docs/methods.md: exit() is implemented but uses std::exit(0) instead of graceful exit

### Interpreter Documentation Issues
- [ ] docs/interpreter/README.md describes CLI arguments but no main.cpp file exists yet

## Code Issues

### Semantic/Logic Issues
- [ ] parser.cpp line 21: Constructor doesn't check if lexer_.next_token() is an error before using value()
- [ ] parser.cpp line 262: parse_function_def() parses return type BEFORE parameters (docs show it should be AFTER: `function name(params) returns type`)
- [ ] runtime.cpp line 24-39: run() method handles control flow returns but doesn't properly handle all ExecFlow cases consistently
- [ ] runtime.cpp line 521-529: fstring implementation is 1-based indexing but accesses eval_args incorrectly (should be num-1 not num as is, and bounds check is wrong)
- [ ] runtime.cpp line 556: Function argument binding uses index instead of proper parameter order preservation
- [ ] lexer.cpp line 292-295: read_string() breaks on unterminated string but doesn't return error - caller checks crudely
- [ ] lexer.cpp line 310-325: read_regex() similar issue with unterminated regex
- [ ] lexer.cpp line 360-365: Crude check for unterminated string "if lexeme.size() < 2" is fragile

### Missing Implementations
- [ ] Variable declaration statements not implemented (only assignment exists)
- [ ] List literal parsing not implemented
- [ ] Type casting/conversion not implemented
- [ ] Regex methods (.getAll, .re, .flags) not implemented
- [ ] List methods (.get) not implemented
- [ ] Match type methods (.start, .end, .content) not implemented as accessible members
- [ ] No builtin functions: setEncoding, clipboard_*, logging, alerts
- [ ] No proper span tracking in Expr nodes (span field exists but not populated in most cases)

### Type System Issues
- [ ] runtime.h line 30: MethodRepr uses vector of pairs for args which doesn't preserve insertion order reliably
- [ ] runtime.cpp line 556: Argument binding iteration assumes map order matches parameter order
- [ ] types_utils.hpp: matches_type() allows implicit int<->float but should be more strict based on docs

### Error Handling Issues
- [ ] lexer.cpp: Unterminated string/regex errors are crude and detected late
- [ ] parser.cpp line 21: No error handling for initial token fetch
- [ ] runtime.cpp: Many operations return RuntimeValue::Null{} silently instead of errors
- [ ] runtime.cpp line 193: Variable lookup returns null instead of "undefined variable" error

### Memory/Performance Issues
- [ ] runtime.cpp line 59-85: eval_statements copies Statement objects from unique_ptrs (inefficient)
- [ ] parser.cpp: Many Result<T> returns by value could be more efficient
- [ ] ast.cpp: Copy constructors use std::visit which allocates unnecessarily

## Weird/Non-Idiomatic Code

### Naming Inconsistencies

- [ ] runtime.h uses `envPtr` typedef (camelCase) but should be `env_ptr` (snake_case) per style guide
- [ ] types_utils.hpp: operator== is defined for RuntimeValue but operator!= is not (should define both or use = default)

### Code Smells
- [ ] runtime.cpp line 177: eval_expr uses lambda `make_err` wrapper which is unnecessary
- [ ] runtime.cpp line 205-253: Short-circuit evaluation could be cleaner with early return pattern
- [ ] lexer.cpp line 293-330: read_string and read_regex have duplicated escape handling logic
- [ ] parser.cpp line 262-281: parse_function_def has confusing control flow for return type parsing
- [ ] runtime_utils.cpp line 98-102: numeric_pow uses manual loop for integer exponentiation instead of std::pow for all cases
- [ ] ast.cpp line 68-78: Statement copy constructor has unnecessarily complex lambda for elif copying

### Unused/Dead Code
- [ ] lexer.h line 91: Token::copy_lexeme() method is unused
- [ ] result.hpp line 54-57: error() accessors return shared_ptr but could return const Error&
- [ ] runtime_utils.h line 20-21: bool_and, bool_or declared but not used (runtime.cpp implements short-circuit directly)

### Missing const-correctness
- [ ] parser.h: Most Parser methods should be const where appropriate
- [ ] runtime.h: Interpreter::eval_statements takes non-const Statement& reference but only reads
- [ ] runtime.h: Interpreter::eval_expr takes non-const Expr& reference but only reads
- [ ] lexer.cpp: read_* methods should be const

### Poor Encapsulation
- [ ] runtime_value.h: All struct members are public (should use accessors)
- [ ] ast.h: All struct members are public (should use accessors)
- [ ] runtime.h: Environment exposes variables map publicly

## Other Improvements

### Feature Gaps (based on docs)
- [ ] No support for regex literals with backslash syntax `\pattern\` (docs say regexes use `\ \`)
- [ ] No support for multi-line strings with line continuation (docs describe `\` before newline)
- [ ] No support for alternative string escaping (only handles basic escapes)
- [ ] fstring implementation doesn't match docs exactly (positional indices off by one)

### Testing Needs
- [ ] No test files found - need unit tests for lexer, parser, runtime
- [ ] Need integration tests for language features
- [ ] Need tests for error cases

### Architecture Issues
- [ ] No separation between parsing and AST construction
- [ ] No symbol table or scope management beyond runtime Environment
- [ ] No type checking phase (type errors only caught at runtime)
- [ ] No optimization passes