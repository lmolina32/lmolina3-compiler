#!/bin/bash

set -e

make bminor
chmod +x ./test/book_test_cases/parser/test_parser.sh

echo "Running book test cases"
echo "-----------------------"
./test/book_test_cases/parser/test_parser.sh
