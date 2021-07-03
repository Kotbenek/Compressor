make all
./bin/compressor
hexdump -C testfile.huffman
hexdump -C decompressed.txt

