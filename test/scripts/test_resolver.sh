#!/bin/bash

# run resolver tests

GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'

for testfile in ./test/resolver/good*.bminor
do
	if ./bin/bminor --resolve $testfile &> $testfile.out
	then
		echo -e "$testfile ${GREEN}success${NC} (as expected)"
	else
		echo -e "$testfile ${RED}failure${NC} (INCORRECT)"
	fi
done

for testfile in ./test/resolver/bad*.bminor
do
	if ./bin/bminor --resolve $testfile &> $testfile.out
	then
		echo -e "$testfile ${GREEN}success${NC} (INCORRECT)"
	else
		echo -e "$testfile ${RED}failure${NC} (as expected)"
	fi
done
