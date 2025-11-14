#!/bin/bash

set -e

BOLD=$(tput bold)
NORMAL=$(tput sgr0)

echo -e "${BOLD}Running All Tests${NORMAL}"
echo ""

for test in encode scanner parser printer resolver; do
    echo -e "${BOLD}${test^} Tests${NORMAL} "
    echo "------------------"
    ./test/scripts/test_${test}.sh
    echo
done
