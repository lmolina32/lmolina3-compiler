#!/bin/bash

# run printer tests

GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'

echo "Pretty print files pass to parser"
for testfile in ./test/printer/good*.bminor
do
    ./bin/bminor --print $testfile &> $testfile.printer.out
	if ./bin/bminor --parse $testfile.printer.out &> /dev/null
	then
		echo -e "$testfile.printer.out ${GREEN}success${NC} (as expected)"
	else
		echo -e "$testfile.printer.out ${RED}failure${NC} (INCORRECT)"
	fi

    ./bin/bminor --print $testfile.printer.out &> $testfile.printer.printer.out
	if cmp -s "$testfile.printer.out" "$testfile.printer.printer.out"; 
	then 
		echo -e "Printer outputs are the same ${GREEN}success${NC} (as expected)"
	else 
		echo -e "Printer outputs are not the same ${RED}failure${NC} (INCORRECT)"
	fi

done
