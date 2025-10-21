#!/bin/bash

# run scanner tests

for testfile in ./test/scanner/good*.bminor
do
	if ./bminor --scan $testfile &> $testfile.out
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done

for testfile in ./test/scanner/bad*.bminor
do
	if ./bminor --scan $testfile &> $testfile.out
	then
		echo "$testfile success (INCORRECT)"
	else
		echo "$testfile failure (as expected)"
	fi
done