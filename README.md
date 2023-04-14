# Compressor
C++ file compression utility tool

## Setup
Run `make build` to compile `./bin/compressor`

## Usage
```
Usage: compressor (-c | --compress | -d | --decompress) (-a <algorithm> | --algorithm <algorithm>) (-i <file> | --input <file>) (-o <file> | --output <file>)

-c, --compress                            Compress file
-d, --decompress                          Decompress file
-a <algorithm>, --algorithm <algorithm>   Algorithm to use
-i <file>, --input <file>                 Input file
-o <file>, --output <file>                Output file

Supported algorithms:
huffman  Canonical Huffman
lz77     Lempel-Ziv 77
lz78     Lempel-Ziv 78
lzw      Lempel-Ziv-Welch
```

## Tests
Run `./test.sh` to launch tests.
Run `./build_and_test.sh` to compile `./bin/compressor` and launch tests.

### Example test output
```
[PASS] huffman    text file      build_and_test.sh
[PASS] huffman    text file      test.sh
[PASS] huffman    text file      makefile
[PASS] huffman    text file      single_char_1
[PASS] huffman    text file      single_char_2
[PASS] huffman    text file      single_char_3
[PASS] huffman    text file      single_char_4
[PASS] huffman    text file      single_char_5
[PASS] huffman    text file      single_char_6
[PASS] huffman    text file      single_char_7
[PASS] huffman    text file      single_char_8
[PASS] huffman    text file      single_char_9
[PASS] huffman    text file      single_char_10
[PASS] huffman    text file      single_char_11
[PASS] huffman    binary file    zero_4K
[PASS] huffman    binary file    random_4K
[PASS] huffman    binary file    zero_256K
[PASS] huffman    binary file    random_256K
[PASS] lz77       text file      build_and_test.sh
[PASS] lz77       text file      test.sh
[PASS] lz77       text file      makefile
[PASS] lz77       text file      single_char_1
[PASS] lz77       text file      single_char_2
[PASS] lz77       text file      single_char_3
[PASS] lz77       text file      single_char_4
[PASS] lz77       text file      single_char_5
[PASS] lz77       text file      single_char_6
[PASS] lz77       text file      single_char_7
[PASS] lz77       text file      single_char_8
[PASS] lz77       text file      single_char_9
[PASS] lz77       text file      single_char_10
[PASS] lz77       text file      single_char_11
[PASS] lz77       binary file    zero_4K
[PASS] lz77       binary file    random_4K
[PASS] lz77       binary file    zero_256K
[PASS] lz77       binary file    random_256K
[PASS] lz78       text file      build_and_test.sh
[PASS] lz78       text file      test.sh
[PASS] lz78       text file      makefile
[PASS] lz78       text file      single_char_1
[PASS] lz78       text file      single_char_2
[PASS] lz78       text file      single_char_3
[PASS] lz78       text file      single_char_4
[PASS] lz78       text file      single_char_5
[PASS] lz78       text file      single_char_6
[PASS] lz78       text file      single_char_7
[PASS] lz78       text file      single_char_8
[PASS] lz78       text file      single_char_9
[PASS] lz78       text file      single_char_10
[PASS] lz78       text file      single_char_11
[PASS] lz78       binary file    zero_4K
[PASS] lz78       binary file    random_4K
[PASS] lz78       binary file    zero_256K
[PASS] lz78       binary file    random_256K
[PASS] lzw        text file      build_and_test.sh
[PASS] lzw        text file      test.sh
[PASS] lzw        text file      makefile
[PASS] lzw        text file      single_char_1
[PASS] lzw        text file      single_char_2
[PASS] lzw        text file      single_char_3
[PASS] lzw        text file      single_char_4
[PASS] lzw        text file      single_char_5
[PASS] lzw        text file      single_char_6
[PASS] lzw        text file      single_char_7
[PASS] lzw        text file      single_char_8
[PASS] lzw        text file      single_char_9
[PASS] lzw        text file      single_char_10
[PASS] lzw        text file      single_char_11
[PASS] lzw        binary file    zero_4K
[PASS] lzw        binary file    random_4K
[PASS] lzw        binary file    zero_256K
[PASS] lzw        binary file    random_256K
72/72 tests passed
```
