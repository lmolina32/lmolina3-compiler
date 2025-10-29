#!/bin/bash

# run printer tests

GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'

echo "Pretty print files pass to parser"
for testfile in ./test/printer/good*.bminor
do
    ./bin/bminor --print $testfile &> $testfile.printer.out
	if ./bin/bminor --parse $testfile.printer.out &> $testfile.printer_to_parse.out
	then
		echo -e "$testfile.printer.out ${GREEN}success${NC} (as expected)"
	else
		echo -e "$testfile.printer.out ${RED}failure${NC} (INCORRECT)"
	fi
done
