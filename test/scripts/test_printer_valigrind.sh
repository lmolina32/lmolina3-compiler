#!/bin/bash

# run valgrind on printer tests

for testfile in ./test/printer/good*.bminor
do
	valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --print $testfile &> $testfile.valgrind.out
	echo -e "Valgrind ran on $testfile"
done
