# TODOS

note: finished todos be removed in most cases, not checked off.

## most important

- 

## dev-related

- [ ] set up and document CMAKE
- [ ] define interpreter behaviour (cli arguments, feedback?)
- [ ] plan helper tool that allows to easily call scripts on shortcuts or per taskbar or hwatever

## smaller todos
- [ ] lexer treats any / that is not // as the start of a regex => division operator is unreachable.
Solution: either check if current expression has a closing \ tag => may be complicated to implement OR change division operator => simpler but makes it more unintuivite
- [ ] add fstring to parser
- [ ] lexer consumes backslash + next-char inside strings but does not collapse backslash + newline into "no newline" at lexing time
- [ ] change type of Token.lexeme to std::string? 
- [ ] Function call argument count mismatch returns OK / null -> change to error 
- [ ] numeric_pow might overflow -> irrelevant? (a) when would that matter (b) it's for the user to worry about -> add documentation when it will overflow

## new classes

- [ ] parser
- [ ] main entry point

## general behaviour

- [ ] add '-' as unary operator (numerical negation)
- [ ] exit() calls std::exit(0) internally -> change to graceful exit
- [ ] builtin methods (docs/methods.md) -> docs need to be fully fleshed out for ALL builtin methods, builtins must be implemented -> seperate /builtins class and/or folder
