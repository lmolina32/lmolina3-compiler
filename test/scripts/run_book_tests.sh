#!/bin/bash

set -e

BOLD=$(tput bold)
NORMAL=$(tput sgr0)

echo -e "${BOLD}Running All Book Tests${NORMAL}"
echo ""

for test in parser printer typecheck; do
    echo -e "${BOLD}${test^} Book Tests${NORMAL} "           
    echo "------------------"
    ./test/book_test_cases/scripts/test_${test}.sh
    echo
done
