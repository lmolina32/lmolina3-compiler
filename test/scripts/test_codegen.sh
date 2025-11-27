#!/bin/bash

# run codegen tests
GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'  

for testfile in ./test/codegen/good*.bminor
do
    number=$(echo $testfile | grep -Eo "[0-9]{2,3}")
	if ./bin/bminor --codegen $testfile ./test/codegen/good$number.s &> $testfile.out
	then
		echo -e "$testfile success ${GREEN}(as expected)${NC} "
	else
		echo -e "$testfile failure ${RED}(INCORRECT)${NC} "
	fi
done