#!/bin/bash

TESTS_DIR="tests"
PASS="[$(tput setaf 2)PASS$(tput sgr0)]"
FAIL="[$(tput setaf 1)FAIL$(tput sgr0)]"
RUNNING="[    ]"

# Printing functions

print_test_init()
{
    echo -n "$RUNNING $1"
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
    if [ "$1" == "$2" ]; then
        print_test_pass
    else
        print_test_fail
        echo "       Arguments are not equal."
        echo "       Argument 1: $1"
        echo "       Argument 2: $2"
    fi
}

# Prepare test directory
if [ -d "$TESTS_DIR" ]; then rm -rf "$TESTS_DIR"; fi
mkdir "$TESTS_DIR"

# Run tests

print_test_init "Huffman test - text file  -  build_and_run.sh"
FILE="build_and_run.sh"
ARG1="${FILE}"
ARG2="${TESTS_DIR}/$(basename ${FILE}).huffman"
ARG3="${TESTS_DIR}/$(basename ${FILE})"
./bin/compressor "-hc" "${ARG1}" "${ARG2}"
./bin/compressor "-hd" "${ARG2}" "${ARG3}"
assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"

print_test_init "Huffman test - text file  -  test.sh"
FILE="test.sh"
ARG1="${FILE}"
ARG2="${TESTS_DIR}/$(basename ${FILE}).huffman"
ARG3="${TESTS_DIR}/$(basename ${FILE})"
./bin/compressor "-hc" "${ARG1}" "${ARG2}"
./bin/compressor "-hd" "${ARG2}" "${ARG3}"
assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"

print_test_init "Huffman test - text file  -  makefile"
FILE="makefile"
ARG1="${FILE}"
ARG2="${TESTS_DIR}/$(basename ${FILE}).huffman"
ARG3="${TESTS_DIR}/$(basename ${FILE})"
./bin/compressor "-hc" "${ARG1}" "${ARG2}"
./bin/compressor "-hd" "${ARG2}" "${ARG3}"
assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"

print_test_init "Huffman test - binary file - zero_4K"
FILE="${TESTS_DIR}/zero_4K"
head -c 4KB /dev/zero > ${FILE}
ARG1="${FILE}"
ARG2="${TESTS_DIR}/$(basename ${FILE}).huffman"
ARG3="${TESTS_DIR}/$(basename ${FILE})"
./bin/compressor "-hc" "${ARG1}" "${ARG2}"
./bin/compressor "-hd" "${ARG2}" "${ARG3}"
assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"

print_test_init "Huffman test - binary file - random_4K"
FILE="${TESTS_DIR}/random_4K"
head -c 4KB /dev/urandom > ${FILE}
ARG1="${FILE}"
ARG2="${TESTS_DIR}/$(basename ${FILE}).huffman"
ARG3="${TESTS_DIR}/$(basename ${FILE})"
./bin/compressor "-hc" "${ARG1}" "${ARG2}"
./bin/compressor "-hd" "${ARG2}" "${ARG3}"
assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"

print_test_init "Huffman test - binary file - zero_256K"
FILE="${TESTS_DIR}/zero_256K"
head -c 256KB /dev/zero > ${FILE}
ARG1="${FILE}"
ARG2="${TESTS_DIR}/$(basename ${FILE}).huffman"
ARG3="${TESTS_DIR}/$(basename ${FILE})"
./bin/compressor "-hc" "${ARG1}" "${ARG2}"
./bin/compressor "-hd" "${ARG2}" "${ARG3}"
assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"

print_test_init "Huffman test - binary file - random_256K"
FILE="${TESTS_DIR}/random_256K"
head -c 256KB /dev/urandom > ${FILE}
ARG1="${FILE}"
ARG2="${TESTS_DIR}/$(basename ${FILE}).huffman"
ARG3="${TESTS_DIR}/$(basename ${FILE})"
./bin/compressor "-hc" "${ARG1}" "${ARG2}"
./bin/compressor "-hd" "${ARG2}" "${ARG3}"
assert_equal "$(sha256sum ${ARG1} | cut -d ' ' -f 1)" "$(sha256sum ${ARG3} | cut -d ' ' -f 1)"

# Remove test directory
rm -rf "$TESTS_DIR"
