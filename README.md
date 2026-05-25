# Bidimensional Array Parser: [SPEC.md](./SPEC.md)

## Usage
This is not a standalone program.
It is a C parsing function meant to be integrated into your project.

Do not compile it as a separate executable.

Instead, include the source file in your build system (Makefile / CMake / manual compilation) and call the function directly.

## Example Integration
```C
#include "bda.h"

int main() {
    bda_descriptor bda;

    // input buffer containing your encoded 2D array
    uint8_t input[] = "0x1,0x2;0x3,0x4;";

    uint64_t output[4];

    bda.x = 2;
    bda.y = 2;
    bda.n = 4;
    bda.src = (uint64_t)input;
    bda.dest = (uint64_t)output;

    bda_parse(&bda);

    return 0;
}
```

## What this library does
Parses a text-encoded 2D array into a contiguous memory buffer.
Supports:
* Decimal numbers
* Hexadecimal
* Negative decimals
* C style block comment (only the /**/ kind)
* Row/column delimiters using , and ;

## Important note
This module assumes the caller manages memory for dest. No allocation is performed internally. 

## Platform Support

This library is developed and tested on x86-64 (x64) architectures only. 
It makes the following assumptions that may not hold on other platforms:

- Pointers fit in 64 bits (`src` and `dest` are stored as `uint64_t`)
- `uint64_t` is 8 bytes (used for memory stride calculations)
- Two's complement signed integer representation