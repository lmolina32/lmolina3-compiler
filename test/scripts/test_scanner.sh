#!/bin/bash

# run scanner tests
GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'  

for testfile in ./test/scanner/good*.bminor
do
	if ./bin/bminor --scan $testfile &> $testfile.out
	then
		echo -e "$testfile ${GREEN}success${NC} (as expected)"
	else
		echo -e "$testfile ${RED}failure${NC} (INCORRECT)"
	fi
done

for testfile in ./test/scanner/bad*.bminor
do
	if ./bin/bminor --scan $testfile &> $testfile.out
	then
		echo -e "$testfile  ${GREEN}success${NC} (INCORRECT)"
	else
		echo -e "$testfile  ${RED}failure${NC} (as expected)"
	fi
done