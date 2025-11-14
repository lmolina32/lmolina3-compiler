#!/bin/bash

# run typchecker tests
GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'  

for testfile in ./test/typechecker/good*.bminor
do
	if ./bin/bminor --typecheck $testfile &> $testfile.out
	then
		echo -e "$testfile success ${GREEN}(as expected)${NC} "
	else
		echo -e "$testfile failure ${RED}(INCORRECT)${NC} "
	fi
done

for testfile in ./test/typechecker/bad*.bminor
do
	if ./bin/bminor --typecheck $testfile &> $testfile.out
	then
		echo -e "$testfile  success ${RED}(INCORRECT)${NC} "
	else
		echo -e "$testfile  failure ${GREEN}(as expected)${NC} "
	fi
done