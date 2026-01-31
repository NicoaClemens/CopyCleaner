# TODOS

note: finished todos be removed in most cases, not checked off.

## most important

- 

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
- [ ] docs/syntax.md describes variable declaration syntax (int n(4)) but there's no VarDeclaration statement type in AST - only Assignment exists. Need to implement proper variable declarations vs assignments
- [ ] docs/syntax.md says type is "boolean" but the AST uses "Bool" - inconsistency in naming
- [ ] docs/syntax.md describes regex methods (.getAll) and list methods (.get) but these are not implemented in runtime
- [ ] docs/syntax.md describes match type with .start, .end, .content but no member access is implemented in the interpreter
- [ ] docs/syntax.md describes member access (.) but Expr enum has no MemberAccess variant
