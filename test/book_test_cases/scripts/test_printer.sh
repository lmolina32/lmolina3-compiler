#!/bin/bash

# run printer tests

GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'

for testfile in ./test/book_test_cases/printer/good*.bminor
do
	if ./bin/bminor --print $testfile &> $testfile.out
	then
		echo -e "$testfile ${GREEN}success${NC} (as expected)"
	else
		echo -e "$testfile ${RED}failure${NC} (INCORRECT)"
	fi
done


for testfile in ./test/book_test_cases/printer/bad*.bminor
do
	if ./bin/bminor --print $testfile &> $testfile.out
	then
		echo -e "$testfile  ${GREEN}success${NC} (INCORRECT)"
	else
		echo -e "$testfile  ${RED}failure${NC} (as expected)"
	fi
done
