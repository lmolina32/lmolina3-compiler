## Language Features

BMinor is a strictly-typed, C-like language with the following features:

### Data Types

**Atomic Types:**

- `integer` - Signed 64-bit integer
  - Decimal: `123`
  - Hexadecimal: `0x104A3F` (case-insensitive digits, lowercase 'x')
  - Binary: `0b0101110101` (lowercase 'b')
- `double` - IEEE 754 double-precision floating point
  - Standard: `12.34`
  - Scientific: `5.67E1`, `89e-2` (case-insensitive 'e')
- `boolean` - `true` or `false`
- `char` - Single 8-bit ASCII character in single quotes: `'q'`
- `string` - Immutable, null-terminated string in double quotes (up to 255 chars): `"hello"`

**Array Types:**

- `array [size] type` - Fixed-size arrays with runtime bounds checking and `#` operator for length
- `carray [size] type` - C-compatible arrays without bounds checking (unsafe)

**Type Inference:**

- `auto` - Variable type inferred from initializer at compile-time

### Escape Sequences

Strings and characters support these escape codes:

```
\a (bell)    \b (backspace)  \e (escape)     \f (form feed)
\n (newline) \r (return)      \t (tab)        \v (vertical tab)
\\ (backslash) \' (quote)     \" (dbl quote)  \0xHH (hex byte)
```

### Operators

Listed by precedence (highest to lowest):

```
()  []  f()         // grouping, array subscript, function call
++  --              // postfix increment, decrement
#                   // unary array length
-   !               // unary negation, logical not
^                   // exponentiation
*   /   %           // multiplication, division, remainder
+   -               // addition, subtraction
<  <=  >  >=  ==  !=  // comparison
&&                  // logical and
||                  // logical or
=                   // assignment
```

### Control Flow

- `if` / `else if` / `else` statements
- `for` loops (C-style)
- `return` statements
- Code blocks with `{ }`

Note: No `while`, `do-while`, or `switch` statements

### Functions

```bminor
// Function with return value
square: function integer ( x: integer ) = {
    return x^2;
}

// Function with inferred return type
add: function auto ( x: integer, y: integer ) = {
    return x + y;  // returns integer
}

// Void function
printarray: function void ( a: array [] integer ) = {
    i: integer;
    for(i=0; i<#a; i++) {
        print a[i], "\n";
    }
}

// Function prototype (for external C functions)
puts: function void ( s: string );

// Main function (required for complete program)
main: function integer ( argc: integer, argv: carray [] string ) = {
    puts("hello world");
    return 0;
}
```

**Parameter passing:**

- `integer`, `boolean`, `char` - passed by value
- `string`, `array` - passed by reference
- Array parameters omit length: `array [] type`

### Print Statement

Unlike C, `print` is a statement (not a function):

```bminor
print "Temperature: ", temp, " degrees\n";
```

Can print multiple expressions of different types in a comma-separated list.

### Type Safety

BMinor enforces strict type checking:

**Invalid:**

```bminor
x: integer = 65;
y: char = 'A';
if(x > y) ...       // Error: different types

f: integer = 0;
if(f) ...           // Error: not a boolean

y: double = x;      // Error: no implicit conversion
```

**Valid:**

```bminor
x: integer = 3;
y: integer = 5;
b: boolean = x < y; // OK: x<y is boolean

c: char = 'a';
if(c == 'a') ...    // OK: both chars
```

### Comments

```bminor
/* C-style block comment */
// C++-style line comment
```

### Identifiers

- May contain letters, numbers, and underscores
- Must begin with letter or underscore
- Up to 255 characters long
- Cannot use reserved keywords

**Reserved keywords:**

```
array auto boolean carray char else false float double
for function if integer print return string true void while
```

### Complete Example

```bminor
// Factorial function
factorial: function integer ( n: integer ) = {
    if(n <= 1) {
        return 1;
    } else {
        return n * factorial(n-1);
    }
}

// Array processing
sum_array: function integer ( arr: array [] integer ) = {
    total: integer = 0;
    i: integer;
    for(i=0; i<#arr; i++) {
        total = total + arr[i];
    }
    return total;
}

// Main program
main: function integer () = {
    numbers: array [5] integer = {1,2,3,4,5};
    result: integer = sum_array(numbers);
    print "Sum: ", result, "\n";
    print "Factorial(5): ", factorial(5), "\n";
    return 0;
}
```
