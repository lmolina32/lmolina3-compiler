#!/bin/bash

set -e

echo "Running book test cases"
echo "-----------------------"

echo "Parser book test cases"
echo "-----------------------"
./test/book_test_cases/scripts/test_parser.sh

echo ""
echo "Printer book test cases"
echo "-----------------------"
./test/book_test_cases/scripts/test_printer.sh
