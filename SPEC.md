# Specification

## Input
* x: Number of columns per row
* y: Number of rows
* n Maximum number of values to attempt to parse
* src: Pointer to input buffer (null terminated string)
* dest: Pointer to output buffer (contiguous uint64_t array size of at least x * y)

## Output
* x: Final column index reached during parsing (never written to)
* y: Final row index reached during parsing (never written to)
* n: The last number when it was not  written or indexed into memory.
* src: Pointer position of where parsing stopped
* dest: Unchanged

## Grammar
The parser will read everything sequencially from src until it either reaches a NULL byte, copies n numbers to memory or finds an out of bounds (per passed x and y) write. The main loop can catch these until it exits (due to the reasons specified above):
1. Comments (It sees "/*")
2. Decimals (It sees [0-9] and sets accumulator to it)
3. Negative Decimals (It sees "-" and sets accumulator to 0)
4. Hex Numbers (It sees "0x" and sets accumulator to 0)
5. An x flush (It sees a ",")
6. A y flush (It sees a ";")
7. Junk (Anything else)

### On comments
It will skip everything until it finds a "*/"

### On Decimals
It will catch:
1. Comments
2. Digits (which it accumulates to what was already accumulated * 10)
3. An x flush
4. A y flush
5. Junk (which gets skipped)

### On Negative Decimals:
It will catch:
1. Comments
2. Digits (which it accumulates to what was already accumulated * 10)
3. An x flush
4. A y flush
5. Junk

### On Hex Numbers
It will catch:
1. Comments (which get skipped)
2. [0-9][a-f][A-F] (which it will accumulate to what was already accumulated * 16)
3. An x flush
4. A y flush
5. Junk

### On an x flush
1. It checks if the x we are currently pointing at (counting from 0) is smaller than the x provided in the descriptor.
2. If not, it uses n as a flag, sets the writes the current number of numbers written to the descriptor's n (so when it returns to the main loop the main loop finishes)
3. If yes, it writes the accumulator to (x;y), increments x, increments the number of numbers written and clears the accumulator.

## On a y flush
1. It checks if the y we are currently pointing at is smaller than the y provided by the descriptor
2. If not, it sets n = number of numbers
3. If yes, it writes the accumulator to (x;y), increments y, clears x, increments the number of numbers written and clears the accumulator.

## On junk
It skips (junk is anything that it did not catch before reaching that point)

## Examples {x, y, n, I/O} 

1) 
input: {1, 1, 1, "123, 124\0"}
output: {1, 0, 0, [[123]]}
note: 123 is parsed, , flushes x to 1, limit n=1 is hit. 124 is never touched.

2) 
input: {2, 2, 4, "1, 2; 3, 7\0"}
output: {1, 1, 7, [[1, 2], [3]]}
Note: 1, 2; completes the first row. 3 is parsed and , flushes it. 7 is read, but since no ; or , follows it before the NULL byte, it remains in the "last number" state and isn't indexed into dest.

3)
input: {2, 2, 4, "1, 2; 3, 4;\0"}
output: {0, 2, 0, [[1, 2], [3, 4]]}
note: The final ; flushes the 4 into memory, resets x to 0, and increments y to 2

4) 
input: {1, 1, 2, "10, 20"}
output: {1, 0, 20, [[10]]}
note: 10 gets read and written, 20 gets read, but since no ; or , follows it before the NULL byte, it gets sent to n.

5)
input: {4, 1, 4, "1, 1jun k2, 23, 43;\0"}
output: {0, 1, 0, [[1, 12, 23, 43]]}
note: The junk inside the "12" gets skipped.

6)
input: {4, 1, 4, "1, 12, 2/*comment123*/3, 43;\0"}
output: {0, 1, 0, [[1, 12, 23, 43]]}
note: Everything inside the comments gets treated as if it never existed.

7)
input: {4, 1, 4, "1, 1junk/*2,*/ 23, 43;\0"}
output: {0, 1, 0, [[1, 123, 43]]}
note: The commas inside the comments and spacing outside the comments do not register as anything. 

8)
input: {4, 1, 4, "1, 1jun k2, 23, 43;\0"}
output: {0, 1, 0, [[1, 12, 23, 43]]}
note: The junk inside the "12" gets skipped.

9) 
input: {2, 3, 9, "1, 52, 47; 132, 4332, 65; 54, 83, 19;\0"}
output: {3, 0, 47, [[1, 52]]}
note: Despite the generous y and n, the moment x overflew the parser returned.

10) 
input: {5, 5, 15, "1, 52, 47; 132, 4332, 65; 54, 83, 19;\0"}
output: {0, 3, 0, [[1, 52, 47], [132, 4332, 65], [54, 83, 19]]}
note: Due to x's value, now the rows have 2 empty cells between each otehr.

###Caution note
Do not write uncommented words between 0x and [,][;] as the characters [a-f][A-F] would get interpreted as part of the hexadecimal. Eg: 0x12hello34 turns into 0x0000000000012E34 but 0x12/*hello*/34 turns into 0x0000000000001234