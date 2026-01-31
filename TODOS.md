# TODOS

note: finished todos be removed in most cases, not checked off.

## most important

- 

## dev-related

- [ ] set up and document CMAKE -> c++20
- [ ] plan helper tool that allows to easily call scripts on shortcuts or per taskbar or whatever

## general todos

- [ ] lexer.cpp uses crude checks for unterminated strings/regexes (acknowledged in comment line 295) - consider more robust validation

## smaller todos


## new classes

- [ ] parser
- [ ] main entry point

## general behaviour

- [ ] exit() calls std::exit(0) internally -> change to graceful exit
- [ ] builtin methods (docs/methods.md) -> docs need to be fully fleshed out for ALL builtin methods, builtins must be implemented -> seperate /builtins class and/or folder

## code quality issues

- [ ] incomplete documentation sections in docs/methods.md: "## Logger" and "## Alert" have no content
