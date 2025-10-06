#!/bin/sh

# run parser tests

for testfile in ./test/parser/good*.bminor
do
	if ./bminor --parse $testfile &> $testfile.out
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done

for testfile in ./test/parser/bad*.bminor
do
	if ./bminor --parse $testfile &> $testfile.out
	then
		echo "$testfile success (INCORRECT)"
	else
		echo "$testfile failure (as expected)"
	fi
done