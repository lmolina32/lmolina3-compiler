#!/bin/bash

set -e

echo "Running All Tests"
echo ""

echo "Encode Tests"
echo "------------------"
./test/scripts/test_encode.sh

echo ""
echo "Scanner Tests"
echo "------------------"
./test/scripts/test_scanner.sh

echo ""
echo "Parser Tests"
echo "------------------"
./test/scripts/test_parser.sh