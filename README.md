# BMinor Compiler

**Author:** Leonardo Molina  
**Course:** [CSE 40243 - Compilers](https://dthain.github.io/compilers-fa25/)

A compiler for the BMinor programming language, built as part of a multi-phase compiler construction project.

## Building

```bash
make           # Build the compiler
make clean     # Remove build artifacts
make help      # Help on make commands
```

The executable will be created at `bin/bminor`.

## Usage

The compiler supports multiple modes of operation:

```bash
# Encode string literals
./bin/bminor --encode <filename.bminor>

# Scan a source file (lexical analysis)
./bin/bminor --scan <filename.bminor>

# Parse a source file (syntax analysis)
./bin/bminor --parse <filename.bminor>

# Print parsed source file (pretty printing)
./bin/bminor --print <filename.bminor>

# Resolve parsed source file (name resolution)
./bin/bminor --resolve <filename.bminor>

# Typecheck resolved source file (type checking)
./bin/bminor --typecheck <filename.bminor>

# Generate code for source file (code generation)
./bin/bminor --codegen <filename.bminor> <output_file.s>
```

### Exit Codes

- `0` - Success
- `1` - Compilation error or invalid input

## Testing

Run the test suites to verify compiler functionality:

```bash
make test-all         # Run all tests (not book test cases)
make test-encode      # Test string encoding
make test-scanner     # Test lexical analysis
make test-parser      # Test syntax analysis
make test-printer     # Test pretty printing
make test-resolver    # Test name resolution
make test-typechecker # Test type checking
make test-codegen     # Test code generation
make test-book        # Run book test cases
```

Test cases are organized in `test/` by compiler phase, with both valid (`good*.bminor`) and invalid (`bad*.bminor`) test programs.

- Current Personal test cases: `test/encoder`, `test/scanner`, `test/parser`, `test/printer`, `test/resolver`, `test/typechecker`, `test/codegen`
- Book test cases: `test/book_test_cases/parser`, `test/book_test_cases/printer`, `test/book_test_cases/typecheck`, `test/book_test_cases/codegen`

## Project Structure

```
bminor/
├── src/
│   ├── main/           # Driver code and main entry point
│   ├── ast/            # Abstract Syntax Tree definitions
│   ├── codegen/        # Scratch register and label functions
│   ├── encoder/        # String literal encoding
│   ├── library/        # Runtime library
│   ├── parser/         # Syntax analysis (Bison)
│   ├── scanner/        # Lexical analysis (Flex)
│   ├── symbol_table/   # Symbol table and scope functions
│   └── utils/          # Utility functions used by compiler
├── test/               # Test cases organized by phase
│   └── scripts/        # Individual test scripts for each phase
├── build/              # Compiled object files (generated)
├── bin/                # Final executable (generated)
└── docs/               # Documentation
```

## Architecture

### Abstract Syntax Tree (AST)

The AST is defined across several header files in `src/ast/`:

- **`decl.h`** - Declarations (global variables and functions)
- **`stmt.h`** - Statements (if-else, for, return, etc.)
- **`expr.h`** - Expressions (operators, literals, function calls)
- **`type.h`** - Type representations (integer, string, boolean, arrays, functions)
- **`param_list.h`** - Function parameter lists

### Symbol Table

The `hash_table` module (`src/symbol_table/`) implements a string-based hash table for symbol management, used to map variable names to their definitions during name resolution and type checking.

### Standard Library

`library.c` contains the BMinor standard library, implementing:

- Print statement support
- Exponentiation operator
- Other runtime support functions

This module is linked against compiled BMinor programs, not the compiler itself.

## Coding Style

To maintain consistency and clarity:

- **Function naming**: Prefix each function with the structure it manipulates
  - Example: `decl_print()`, `expr_evaluate()`, `stmt_typecheck()`
- **File organization**:
  - Function prototypes → `.h` files
  - Function implementations → `.c` files
  - Example: `decl_print()` prototype in `decl.h`, implementation in `decl.c`

## Language Features

BMinor is a strictly-typed, C-like language with the following features:

- **For more detail go to [docs](https://dthain.github.io/compilers-fa25/bminor)**
- **For concise overview go [here](/docs/bminor_language.md)**

See `test/` directories for example programs.

## Requirements

- GCC or compatible C compiler - Version: 8.5.0
- Flex - Version: 2.6.1
- Bison (GNU Bison) - Version: 3.0.4
- GNU bash - Version: 4.4.20(1)-release
- GNU Make - Version: 4.2.1

## Development Notes

See `docs/devel.md` for implementation details and development guidelines.

## License

Academic project for CSE 40243. Do not distribute or copy without permission.

---

_This project follows the structure and guidelines provided by the CSE 40243 course materials._
