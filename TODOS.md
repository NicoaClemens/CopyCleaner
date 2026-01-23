# TODOS

## most important

- [ ] rethink how error handling works & document it better (binary op fall through)
- [ ] change Result to using `Result = std::variant<T, std::shared_ptr<Error>>;` in errors.hpp and change usages -> or as entire class with is_ok(), is_err(), T& unwrap, etc?

## dev-related

- [ ] set up and document CMAKE 

## smaller todos
- [ ] lexer treats any / that is not // as the start of a regex => division operator is unreachable.
Solution: either check if current expression has a closing \ tag => may be complicated to implement OR change division operator => simpler but makes it more unintuivite
- [ ] add fstring to parser
- [ ] lexer consumes backslash + next-char inside strings but does not collapse backslash + newline into "no newline" at lexing time
- [ ] `eval_statements()` in runtime.cpp repeatedly copies `Statement` objects but `Statement` has unique_ptr and is therefore not copy-able => implement deep copy construcotr
- [ ] change type of Token.lexeme to std::string? 
- [ ] Function call argument count mismatch returns OK / null -> change to error 
- [ ] change regex to a struct RegexType with fields for literal and flags instead of std::regex -> also fix regex truthyness equality and to_string
- [ ] numeric_pow might overflow

## new classes

- [ ] parser
- [ ] main entry point

## general behaviour

- [ ] define interpreter behaviour (cli arguments, feedback?)
- [ ] plan helper tool that allows to easily call scripts on shortcuts or per taskbar or hwatever
- [ ] add '-' as unary operator (numerical negation)
- [ ] exit() calls std::exit(0) internally -> change to graceful exit
- [ ] builtin methods (docs/methods.md) -> docs need to be fully fleshed out for ALL builtin methods, builtins must be implemented -> seperate /builtins class and/or folder
