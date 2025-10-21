#!/bin/bash

set -e

echo "Running All Tests"
echo ""

echo "Encode Tests"
echo "------------------"
./test/encode/test_encode.sh

echo ""
echo "Scanner Tests"
echo "------------------"
./test/scanner/test_scanner.sh

echo ""
echo "Parser Tests"
echo "------------------"
./test/parser/test_parser.sh