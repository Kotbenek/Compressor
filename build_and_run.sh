make all
./bin/compressor
cksum testfile.txt
cksum testfile.huffman
cksum decompressed.txt

