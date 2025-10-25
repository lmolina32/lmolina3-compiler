#!/bin/bash

# run printer tests

GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'  

for testfile in ./test/printer/good*.bminor
do
	if ./bin/bminor --print $testfile &> $testfile.out
	then
		echo -e "$testfile ${GREEN}success${NC} (as expected)"
	else
		echo -e "$testfile ${RED}failure${NC} (INCORRECT)"
	fi
done

for testfile in ./test/printer/good*.bminor
do
	valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --print $testfile &> $testfile.valgrind.out
done
