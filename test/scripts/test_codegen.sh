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

		# compile program 
		gcc -g "test/codegen/good$number.s" src/library/library.c -o "test/codegen/good$number.out" 
		compile_status=$?

		if [ $compile_status -ne 0 ]; then
			echo "Compilation failed with status $compile_status" >> "$testfile.out"
		fi

		# Run the program
		./test/codegen/good$number.out &> $testfile.program.out
		run_status=$?
		echo -e "\n---------------------------------------" >> $testfile.program.out 
		echo -e "exit status $run_status" >> $testfile.program.out

	else
		echo -e "$testfile failure ${RED}(INCORRECT)${NC} "
	fi
done