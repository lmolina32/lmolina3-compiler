#!/bin/bash

# run parser tests

for testfile in ./test/book_test_cases/printer/good*.bminor
do
	if ./bin/bminor --print $testfile &> $testfile.out
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done

for testfile in ./test/book_test_cases/printer/bad*.bminor
do
	if ./bin/bminor --print $testfile &> $testfile.out
	then
		echo "$testfile success (INCORRECT)"
	else
		echo "$testfile failure (as expected)"
	fi
done