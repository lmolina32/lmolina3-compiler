# Name: Leonardo Molina

# NetID: lmolina3

# Development Log

## Encoder

### Interpretation

- For encode the values outside the ascii range (e.g not between 32 and 127) will be encoded with the value itself. This means that if the incoming string "\0xff" the decoder will replace the string with the ascii value and the encoder will not encode it back into '0xff' rather it would leave it. This is the interpretation of not turning ascii values outside of the range of 32-127 into hex.
- the following separated by commas are valid strings \"\", "\", "string""string"

### development encoder

- What AI tools (if any) did you use, and what sort of prompts did you provide?
  - did not use any AI tools writing the code itself, used AI for code review
  - Used (gpt-5) for code review:
    - (gpt-5 ) - The code snippet I am given you is the source file for an encoder it has three functions, encode, decode char_to_hex. Its purpose is to encode and decode a programming language with specific specs. The code review should focus on useful comments, error handling and overall structure of the code. If there are any issues with the code e.g potential of a null pointer or improper error handling provide a suggestion on how to implement. Additionally, if code can be simplified provide this suggestion as well. Lastly some other areas it should focus on improving are suggestions on potential bugs, maintainability of the code, security and performance. If you implement the suggestions directly in the code base put them in between tags like this <code> <\code>. First think about how the two functions and helper functions work together. Then think about all the potential issues/improvements you can make on the code (e.g performance and readability) then think on these improvements for 3 minutes. If you have not thought of great suggestions think more until you have improved the functions without taking their purpose away from it. (Then attached encoder.c)
  - Implemented suggestions on combining if statements, simplifying logic, and produce error print statements. Other suggestions were ignored e.g using specific error codes when returning instead of just returning false and using a hash table instead of switch statements.
- What parts of the code were easy to get right?
  - Main - For the main file what was easy to get write was reading in the file and calling the separate functions. Additionally, taking arguments through the command line and then parsing it was fairly straight forward.
  - encoder.c - The encoder was more straight forward than the decoder was. Maybe this was because I tackled the decoder first, but the encoder did not seem as difficult as the decoder nearly was. I think this is because all the edge case testing mainly occurs in the decoder not the encoder.
- What parts were difficult to get right and required more effort?
  - **Edge Cases** - What was hard to get right was the decoder by far. The decoder which initially seemed like an easy task quickly became challenging. Not because of the code itself (besides the hex conversion, initially) but due to edge cases. It became more apparent how strict the encoder needed to be once I started building the decoder.
  - **testing** - Overlooked part for most of my classes since typically the test cases are built for me. However, building the test cases myself forced me to think hard on the language specifics but also on how an end-user would break my decoder/encoder. It was by far the hardest task of the assignment and still needs tons to develop before my test cases have 100% coverage on my code.

## Scanner

### Interpretation

- char is one character version of a str. This means that \c, where c is any printable character would be itself. Additionally, we can valid hex values that can be transcribed from hex to ascii for example '\0x78' is valid. For chars what is not valid is several characters, empty char (e.g ''), or '''.
- strings when scanned must be in ", cannot have new line characters, and " in between (e.g "abc"abc" is not valid). The string is passed into the decoded_string function in `encoder.c` which will determine if it is a valid or invalid string. If it is valid TOKEN_CHAR_LITERAL is returned, if it is not then TOKEN_ERROR is returned.
- for the scanner leading 00s are allowed and will be handled later.
- for double literals they must have a number on both sides of the decimal point, otherwise it is invalid (e.g 12.232 is valid, .12 or 12. is not valid)
- There is not current cutoff on how long integers and doubles should be, this will be taken care of down the line
- invalid comments are not returned as errors in the scanner (e.g /\* not closed comment will be parsed into several different tokens). This was discussed in the Slack.
- H, B must be capitalized for the compiler to read hexadecimal and binary values
- Doubles have two tokens TOKEN_DOUBLE_LITERAL, TOKEN_DOUBLE_SCIENTIFIC_LITERAL.
- integers have three tokens TOKEN_INTEGER_LITERAL, TOKEN_BINARY_LITERAL, TOKEN_HEXIDECIMAL_LITERAL
- the scanner does not produce errors for invalid code at this point. For example, `2132232hbinary` is not valid in most programming languages as an integer. But, for the scanner it would read it as `TOKEN_INTEGER_LITERAL` and `TOKEN_IDENTIFIER`. This is what happens for unclosed comments.
- Lastly, comments are nongreedy meaning that `/* /* */ */` would only capture `/* /* */` and the last will be read as `TOKEN_MULTIPLICATION` and `TOKEN_DIVISION`.

### Structure

- `bminor.c` - contains the argument parser and calls functions from bminor_functions
- `bminor_functions.c` - contains what occurs when you run `bminor --scan file` or `bminor --encode file`. This is to make the code more maintainable but also separate behavior of functions.

### Development code

- for the scanner I didn't use any AI, but did ask Prince for help on what is valid and not valid in the scanner.
- What was easy to get right was the main bminor file, setting up the --scan and then just reading the input from yylex() and printing out the tokens was pretty easy. Additionally, I moved around my code to make it more readable and understandable.
- The scanner.flex file is the one that gave me to most trouble. We skimmed over the implementation in class so it wasn't fresh in my memory. The flex scanner generator was sort of helpful sort of not. But, once getting started it was easy to figure out how to work everything together. But, the regex for the strings and the comments were the most trickiest part I believe to get right.

## Parser

### Interpretation

- I took the literal sense of what the spec told us is valid and not valid. For the case of what a function returns I did not allow for the function to return arrays or arrays of functions. Rather I only allowed for them to return the atomic types, auto and or void. Every other type would fail in this case.

### structure

- `tokens.h` is now `tokens_to_string.c` this was kept to still be able to easily print out all the tokens for the scanner if you wanted to run it.

### development of code

- I didn't use any ai tools to complete the assignment
- What was easy was to set up the tokens, and the process of linking all the files together in order to be able to run `yyparse()`. It was also pretty easy fixing some of the initial issues I had when I was creating the `token.h` file instead of having it already made.
- The hardest part by far was writing out the grammar in the bison file. It took an extremely long time to be able to get down to a solution that works on the test cases. I had several reduce/reduce conflicts and shift/reduce conflicts that were difficult do deal with and handle. After some time of working on it I was able to get a running parser that is able to run what the specification of the language specify.

## Printer

### structure

```
bminor/
├── src/
│   ├── main/           # Driver code and main entry point
│   ├── scanner/        # Lexical analysis (Flex)
│   ├── parser/         # Syntax analysis (Bison)
│   ├── ast/            # Abstract Syntax Tree definitions
│   ├── encoder/        # String literal encoding
│   ├── library/        # Symbol table and runtime library
│   └── utils/          # utility functions used by compiler
├── test/               # Test cases organized by phase
│   └── scripts/        # Individual test scripts for each phase
├── build/              # Compiled object files (generated)
├── bin/                # Final executable (generated)
└── docs/               # Documentation
```

- executable bminor is made in `bin/bminor` or just in the plain directory `bminor`. The test scripts call ./bin/bminor but they are the same program (e.g they are compiled the same)
- **Testing** - there are two extra scripts written
  - `test_printer_to_parser.sh` which runs the printer and sets the output to `$testfile.printer.out`. Then is runs it through the parser to see if it parses correctly and prints out if it did or didn't. Then if reruns the printer with the `$testfile.printer.out` which outputs `$testfile.printer.printer.out`. Finally, it compares if these two files are the same if prints out the files are the same, if not it doesn't.
  - `test_printer_to_valgrind.sh`. This runs the printer operation with `--leak-check=full --show-leak-kinds=all -s` to ensure everything is allocated and deallocated at from the heap.

### Development of code

- used AI to rewrite README.md and create /docs/bminor_language.md
  - feed in the old readme and prompted the LLM to make a more concise and clear readme. Explained the current steps I have implemented in the compiler as well as passed in my code structure.
- used AI to find best structure to use when implementing precedence. The issue was that I wrote the enum out of order and needed a way to pass the enum value like `EXPR_ADD` and get the precedence value associated with that expression. The prompt I gave is:
  - I have a typedef enum that has a bunch of precedence rules like expr_add, expr_sub, expr_group i want to create another structure that has all the values tied to their precedence going from lower precedence to higher precedence and I want to use the enum value to index or get the value of precedence how could you implement something like this in C?
  - from this prompt I learned you can implement arrays with enums values and everything else is set to 0, e.g (`static const int expr_precedence[EXPR_COUNT] = { [EXPR_ADD] = 1}`)
- what was easy was to set up all the functions initially like `expr_create, decl_create` etc. Additionally, it was relatively easy to set up the bison semantic actions for most of the rules. For most of the printing it was pretty straight forward initially with the printing for all of the different structures in the AST.
- The hardest part initially was getting the `parser.bison` to run. I rewrote the header files to use `typedef` for all the structs and for some reason `parser.bison` was not reading in the structures correctly. The issue was fixed but it took a while to get it to work with forward declarations and forcing the header files to be in the `parser.bison` file with `%code requires`. Additionally, the precedence printing was extremely confusing and hard to figure out. I iterated through several different ideas and fell back to the original idea of walking down the groups and determining precedence. The tricky part was figuring out I also had to take into account the associativity of different operators. Additionally, while pretty printing I ran into issues on my semantic actions which were tricky to figure out, locate and then fix. Additionally, I had issues with getting the correct tokens from `yytext` but the issue was easy to fix once I found the correct stack overflow page that talked about the issue.

## Resolver

### Structure

- `src/symbol_table` was created which is where the `scope.c` and `hash_table.c` live.

### Notes

- which count resets from params and locals as discussed in class. We discussed in class that for params you enter a scope and then when you enter the function body you enter a new scope as well. This means the params which count starts at 0 and the local which count starts at 0. But, in the function body you do not reset the which count when entering a new scope for if-else or for loops.
- For bminor you can have several function prototypes with the same name. The types are not checked so at this stage you can have functions with the same name returning different types and having different parameters ( this will be handled in the type checker). But, you cannot have multiple function definitions so once you init a function with braces you will get a resolver error for using the same function name and initializing it with braces and stmt block.

### Development of code

- No AI was used for creating the code.
- What was easy was initially create all the scope functions and walking the tree to create the name resolution. I thought it was pretty straight forward to figure out how to push things onto the symbol table when to enter and leave scopes and when to name resolve.
- The hard part began when I started dealing with the which for locals and params. This took me a while to figure out where and when to update the which statement. Additionally, the functions prototypes and the function declarations were though to get right. Initially I thought the resolver was super easy but dealing with these two issues were difficult to figure out and to ensure they were working together.

## Typechecker

### notes

- Most of the typechecking rules came form chapter 7 pages 106 and 107. It was expanded since the compiler for 2025 has more types than the book originally had.
- For oddities that should be covered
  - Functions that return arrays or other functions. -> this is covered in the parse as I did not allow it in my grammar per the spec
  - Functions as parameters to other functions. -> this is covered in the parser as I did not allow it in my grammar per the spec
  - Arrays of functions -> this is caught by typechecking if passing in just the name, if doing a function call (e.g f()) it will fail in the parser
  - Non-constant initializers for global variables. -> this is done with typechecking
  - Array initializers for local variables. -> this is done with typechecking
- Lastly you cannot define function with auto as a argument as said in the spec.

### Development of code

- Used AI to think of corner cases of auto and different expressions
  - prompt: When building a compiler for a toy language that employs carrays, arrays, boolean, integer, string, char and auto what are corner cases that you must considered when performing typechecking? What are the different common practices when handling these type of checks and how should one format the error messages to be clear and concise?
- The typechecker is easy if you have the right idea down. Having the book cover some of the cases and what we did in class gave me a good starting point in writing the typechecker. Other than that the easiest part was writing the code when you know what you needed to test and what edge cases you needed to cover. I also thought that the general structure was easy to come up with since this is the third deliverable where we traverse the AST, so I had no questions there. Lastly, building the general structure (e.g switch cases for STMTS and EXPRS, decl functions and non-functions) was pretty clear and easy to set up.
- the hardest part of the typechecker was all the different cases you had to consider when building it. It was hard to think of the cases to begin with but a lot of code overlapped especially in the expr_typecheck. But it became more tricky and harder to handle and think of different corner cases for when you introduce auto. I think this was the trickiest part of the typechecker and coming up with different cases for all the different operators, functions, decls and statements.

## Code Generation

### Notes

- Due to the large number of X86-64 registers, you may use a simple non-spilling register allocator, and fail with an “out of registers” error on really complicated nested expressions.
  Calls to functions with more than six arguments may fail with a “too many arguments” error.
- Arrays at local scope and multi-dimensional arays may fail with an error of “not implemented”. (However, you must support global one-dimensional arrays (and carrays) and passing arrays (and carrays) as function arguments.)
- Floating point types may fail code generation with an “floating point not not supported” error.
- **All the above requirements are upheld in the code generation**
- Note: arrays are able to be printed. Decls of Array type print out the contents since the length of the array is stored at the beginning. Carrays cannot be printed, rather what is printed is the address of the carray. This was done because there was no way of determining the size of the carray when using it in argv.

### Structure

- `src/codegen` was created which is where the support libraries of the code generation live. The addition not included in class is `str_lit`. This library creates a linked list of all the string literals in the program.

### Development of code

- No AI was used for the development of the code. Once the code was fully developed and passed all the book test cases and my personal test cases I asked Gemini o3 to perform a code review on the code generation functions. The prompt given was 
  - I am going to give you code for my code generation I cannot give the entirety of the source files because the context window will be to large. Rather I want you to perform a code review on the functions that I pass to you and critique the code, modularity, readability, security and performance, any suggestions you have tell me and explain why. 
  
- The parts of the code to get easy were the parts provided in class and in the book. These were easy to get started with and see how the general structure of the code generation would take place. Additionally, the simplifying of requirements greatly helped in reducing the load of building the code generation. While there was still a lot of moving pieces and thought had to be poured in to solve all the different cases. The removing of some of the features made the code generation more manageable than overwhelming. Additionally, once I got a good understand of the x86 structure and what is expected, most of expr_codegen, stmt_codegen, and decl_codegen was straight forward and easy to understand.
- I think the hardest and most challenging parts of the code were the arrays and the strings. This is because with arrays you weren't containing the values but rather the address of the array. Then you had to know when you had to index into the array and what index represented. This took some time to figure out especially since type arrays have the length at the beginning of the address. Once arrays were figured out the last challenge was strings. I had to come up with a way to put all the string literals in the .data segment of the x86 file. This was easy to think of -> I thought just add them to a linked list as you code generate the file then add them to the .data segment. The challenging part was keeping track of all the string literals and then placing the correct labels in place of the string literals that were placed into the registers. This took time to figure out and find the most efficient way of solving this. But once I got the general idea, it was pretty simple to implement besides the common segfault.
