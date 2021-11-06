#!/bin/bash

TESTS_DIR="tests"

PASS="[$(tput setaf 2)PASS$(tput sgr0)]"
FAIL="[$(tput setaf 1)FAIL$(tput sgr0)]"
RUNNING="[    ]"

TESTS=0
TESTS_PASSED=0

# Printing functions
print_test_init()
{
    echo -n -e "$RUNNING $1"
}

print_test_pass()
{
    echo -n -e "\r"
    echo "$PASS"
}

print_test_fail()
{
    echo -n -e "\r"
    echo "$FAIL"
}

# Testing functions
assert_equal()
{
    ((TESTS=TESTS+1))
    if [ "$1" == "$2" ]; then
        ((TESTS_PASSED=TESTS_PASSED+1))
        print_test_pass
    else
        print_test_fail
        echo "       Arguments are not equal."
        echo "       Argument 1: $1"
        echo "       Argument 2: $2"
    fi
}

test_compress_decompress_huffman()
{
    ARG1="${FILE}"
    ARG2="${TESTS_DIR}/$(basename ${FILE}).huffman"
    ARG3="${TESTS_DIR}/$(basename ${FILE}).decompressed"
    
    ./bin/compressor "-c" "huffman" "${ARG1}" "${ARG2}"
    ./bin/compressor "-d" "huffman" "${ARG2}" "${ARG3}"
    
    assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"
    
    rm ${ARG2}
    rm ${ARG3}
}

test_compress_decompress_lz77()
{
    ARG1="${FILE}"
    ARG2="${TESTS_DIR}/$(basename ${FILE}).lz77"
    ARG3="${TESTS_DIR}/$(basename ${FILE}).decompressed"
    
    ./bin/compressor "-c" "lz77" "${ARG1}" "${ARG2}"
    ./bin/compressor "-d" "lz77" "${ARG2}" "${ARG3}"
    
    assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"
    
    rm ${ARG2}
    rm ${ARG3}
}

# Prepare test directory
if [ -d "$TESTS_DIR" ]; then rm -rf "$TESTS_DIR"; fi
mkdir "$TESTS_DIR"

# Prepare files
FILE="${TESTS_DIR}/zero_4K"
head -c 4KB /dev/zero > ${FILE}

FILE="${TESTS_DIR}/random_4K"
head -c 4KB /dev/urandom > ${FILE}

FILE="${TESTS_DIR}/zero_256K"
head -c 256KB /dev/zero > ${FILE}

FILE="${TESTS_DIR}/random_256K"
head -c 256KB /dev/urandom > ${FILE}

# Run tests
print_test_init "Huffman\ttext file\tbuild_and_test.sh"
FILE="build_and_test.sh"
test_compress_decompress_huffman

print_test_init "Huffman\ttext file\ttest.sh"
FILE="test.sh"
test_compress_decompress_huffman

print_test_init "Huffman\ttext file\tmakefile"
FILE="makefile"
test_compress_decompress_huffman

print_test_init "Huffman\tbinary file\tzero_4K"
FILE="${TESTS_DIR}/zero_4K"
test_compress_decompress_huffman

print_test_init "Huffman\tbinary file\trandom_4K"
FILE="${TESTS_DIR}/random_4K"
test_compress_decompress_huffman

print_test_init "Huffman\tbinary file\tzero_256K"
FILE="${TESTS_DIR}/zero_256K"
test_compress_decompress_huffman

print_test_init "Huffman\tbinary file\trandom_256K"
FILE="${TESTS_DIR}/random_256K"
test_compress_decompress_huffman

print_test_init "LZ77\ttext file\tbuild_and_test.sh"
FILE="build_and_test.sh"
test_compress_decompress_lz77

print_test_init "LZ77\ttext file\ttest.sh"
FILE="test.sh"
test_compress_decompress_lz77

print_test_init "LZ77\ttext file\tmakefile"
FILE="makefile"
test_compress_decompress_lz77

print_test_init "LZ77\tbinary file\tzero_4K"
FILE="${TESTS_DIR}/zero_4K"
test_compress_decompress_lz77

print_test_init "LZ77\tbinary file\trandom_4K"
FILE="${TESTS_DIR}/random_4K"
test_compress_decompress_lz77

print_test_init "LZ77\tbinary file\tzero_256K"
FILE="${TESTS_DIR}/zero_256K"
test_compress_decompress_lz77

print_test_init "LZ77\tbinary file\trandom_256K"
FILE="${TESTS_DIR}/random_256K"
test_compress_decompress_lz77

# Remove test directory
rm -rf "$TESTS_DIR"

# Display test results
echo "${TESTS_PASSED}/${TESTS} tests passed"
