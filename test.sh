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
    local ARG1
    local ARG2
    
    ARG1=${TESTS_DIR}/$(basename "$1").huffman
    ARG2=${TESTS_DIR}/$(basename "$1").decompressed
    
    ./bin/compressor "-c" "huffman" "$1" "${ARG1}"
    ./bin/compressor "-d" "huffman" "${ARG1}" "${ARG2}"
    
    assert_equal "$(sha256sum "$1" | cut -d ' ' -f 1)" "$(sha256sum "${ARG2}" | cut -d ' ' -f 1)"
    
    rm "${ARG1}"
    rm "${ARG2}"
}

test_compress_decompress_lz77()
{
    local ARG1
    local ARG2
    
    ARG1=${TESTS_DIR}/$(basename "$1").lz77
    ARG2=${TESTS_DIR}/$(basename "$1").decompressed
    
    ./bin/compressor "-c" "lz77" "$1" "${ARG1}"
    ./bin/compressor "-d" "lz77" "${ARG1}" "${ARG2}"
    
    assert_equal "$(sha256sum "$1" | cut -d ' ' -f 1)" "$(sha256sum "${ARG2}" | cut -d ' ' -f 1)"
    
    rm "${ARG1}"
    rm "${ARG2}"
}

# Prepare test directory
if [ -d "$TESTS_DIR" ]; then rm -rf "$TESTS_DIR"; fi
mkdir "$TESTS_DIR"

TXT_FILES=("build_and_test.sh" "test.sh" "makefile")
BIN_FILES=()

# Prepare binary files
FILE="${TESTS_DIR}/zero_4K"
head -c 4KB /dev/zero > ${FILE}
BIN_FILES+=("${FILE}")

FILE="${TESTS_DIR}/random_4K"
head -c 4KB /dev/urandom > ${FILE}
BIN_FILES+=("${FILE}")

FILE="${TESTS_DIR}/zero_256K"
head -c 256KB /dev/zero > ${FILE}
BIN_FILES+=("${FILE}")

FILE="${TESTS_DIR}/random_256K"
head -c 256KB /dev/urandom > ${FILE}
BIN_FILES+=("${FILE}")

# Run tests

# Text files - Huffman algorithm
for f in "${TXT_FILES[@]}"; do
    print_test_init "Huffman\ttext file\t$(basename "$f")"
    test_compress_decompress_huffman "$f"
done

# Binary files - Huffman algorithm
for f in "${BIN_FILES[@]}"; do
    print_test_init "Huffman\tbinary file\t$(basename "$f")"
    test_compress_decompress_huffman "$f"
done

# Text files - LZ77 algorithm
for f in "${TXT_FILES[@]}"; do
    print_test_init "LZ77\ttext file\t$(basename "$f")"
    test_compress_decompress_lz77 "$f"
done

# Binary files - LZ77 algorithm
for f in "${BIN_FILES[@]}"; do
    print_test_init "LZ77\tbinary file\t$(basename "$f")"
    test_compress_decompress_lz77 "$f"
done

# Remove test directory
rm -rf "$TESTS_DIR"

# Display test results
echo "${TESTS_PASSED}/${TESTS} tests passed"
