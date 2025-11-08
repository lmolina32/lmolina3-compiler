#!/bin/bash

for testfile in ./test/typechecker/good*.bminor
do
    # Run valgrind, then filter only the desired lines
    valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --typecheck "$testfile" \
        2>&1 | grep -A5 -E "HEAP SUMMARY" > "$testfile.valgrind.out"

    echo "Valgrind ran on $testfile"
    valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --typecheck "$testfile" \
        2>&1 | grep -A5 -E "HEAP SUMMARY"
done

for testfile in ./test/typechecker/bad*.bminor
do
    # Run valgrind, then filter only the desired lines
    valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --typecheck "$testfile" \
        2>&1 | grep -A5 -E "HEAP SUMMARY" > "$testfile.valgrind.out"

    echo "Valgrind ran on $testfile"
    valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --typecheck "$testfile" \
        2>&1 | grep -A5 -E "HEAP SUMMARY"
done
