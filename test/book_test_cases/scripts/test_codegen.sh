#!/bin/bash

# run typechecker tests

GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'  

IFS='.'
for testfile in ./test/book_test_cases/codegen/good*.bminor
do
    number=$(echo $testfile | grep -Eo "[0-9]{2,3}")
	if ./bin/bminor --codegen $testfile ./test/book_test_cases/codegen/$number.s &> $testfile.out
	then
		echo -e "$testfile ${GREEN}success${NC} (as expected)"
	else
		echo -e "$testfile ${RED}failure${NC} (INCORRECT)"
	fi
done