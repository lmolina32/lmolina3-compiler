#!/bin/bash

set -e

make ./bin/bminor
chmod +x ./test/book_test_cases/scripts/*.sh

echo "Running book test cases"
echo "-----------------------"
./test/book_test_cases/scripts/test_parser.sh
