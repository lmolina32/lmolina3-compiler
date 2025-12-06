#!/bin/bash

for testfile in ./test/book_test_cases/codegen/good*.bminor
do
    # Run valgrind, then filter only the desired lines
    valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --codegen "$testfile" ./test/codegen/val.out \
        2>&1 | grep -A5 -E "HEAP SUMMARY" > "$testfile.valgrind.out"

    echo "Valgrind ran on $testfile"
    valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --codegen "$testfile" ./test/codegen/val.out \
        2>&1 | grep -A5 -E "HEAP SUMMARY"
done

for testfile in bounce.bminor maze.bminor fibonnacci.bminor; 
do

    # Run valgrind, then filter only the desired lines
    valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --codegen "./test/book_test_cases/codegen/$testfile" ./test/book_test_cases/codegen/val.out \
        2>&1 | grep -A5 -E "HEAP SUMMARY" > "$testfile.valgrind.out"

    echo "Valgrind ran on $testfile"
    valgrind --leak-check=full --show-leak-kinds=all -s ./bin/bminor --codegen "/test/book_test_cases/codegen/$testfile" ./test/book_test_cases/codegen/val.out \
        2>&1 | grep -A5 -E "HEAP SUMMARY"
done
