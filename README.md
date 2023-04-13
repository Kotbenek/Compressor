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
<img src="images/test_output.png">
