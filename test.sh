#!/bin/bash

TESTS_DIR="tests"

PASS="[$(tput setaf 2)PASS$(tput sgr0)]"
FAIL="[$(tput setaf 1)FAIL$(tput sgr0)]"
RUNNING="[    ]"

TESTS=0
TESTS_PASSED=0

PADDING="                                        "
ALGORITHM_PADDING=10
FILE_TYPE_PADDING=14

# Printing functions
print_test_init()
{
    local algorithm
    local file_type
    local file_name

    algorithm="$1"
    algorithm="${algorithm:0:ALGORITHM_PADDING}${PADDING:0:$((ALGORITHM_PADDING - ${#algorithm}))}"
    file_type="$2"
    file_type="${file_type:0:FILE_TYPE_PADDING}${PADDING:0:$((FILE_TYPE_PADDING - ${#file_type}))}"
    file_name="$3"

    echo -n -e "${RUNNING} ${algorithm} ${file_type} ${file_name}"
}

print_test_pass()
{
    echo -n -e "\r"
    echo "${PASS}"
}

print_test_fail()
{
    echo -n -e "\r"
    echo "${FAIL}"
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

test_compress_decompress()
{
    local ARG1
    local ARG2

    ARG1=${TESTS_DIR}/$(basename "$2")."$1"
    ARG2=${TESTS_DIR}/$(basename "$2")."$1".decompressed

    ./bin/compressor "-c" "-a" "$1" "-i" "$2" "-o" "${ARG1}"
    ./bin/compressor "-d" "-a" "$1" "-i" "${ARG1}" "-o" "${ARG2}"

    assert_equal "$(sha256sum "$2" | cut -d ' ' -f 1)" "$(sha256sum "${ARG2}" | cut -d ' ' -f 1)"

    rm "${ARG1}"
    rm "${ARG2}"
}

# Prepare test directory
if [ -d "${TESTS_DIR}" ]; then rm -rf "${TESTS_DIR}"; fi
mkdir "${TESTS_DIR}"

ALGORITHMS=("huffman" "lz77" "lz78" "lzw")
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

# Prepare additional text files
for i in {1..11}; do
    FILE="${TESTS_DIR}/single_char_${i}"
    for a in $( seq 1 "${i}" ); do
        echo -n "A" >> ${FILE}
    done
    TXT_FILES+=("${FILE}")
done

# Run tests
for a in "${ALGORITHMS[@]}"; do
    # Text files
    for f in "${TXT_FILES[@]}"; do
        print_test_init "${a}" "text file" "$(basename "${f}")"
        test_compress_decompress "${a}" "${f}"
    done

    # Binary files
    for f in "${BIN_FILES[@]}"; do
        print_test_init "${a}" "binary file" "$(basename "${f}")"
        test_compress_decompress "${a}" "${f}"
    done
done

# Remove test directory
rm -rf "${TESTS_DIR}"

# Display test results
echo "${TESTS_PASSED}/${TESTS} tests passed"
