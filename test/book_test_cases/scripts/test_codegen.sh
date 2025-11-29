#!/bin/bash

# run codegen tests
GREEN='\e[32m'
RED='\e[31m'
NC='\e[0m'  

for testfile in ./test/book_test_cases/codegen/good*.bminor
do
    number=$(echo $testfile | grep -Eo "[0-9]{1,3}")
	if ./bin/bminor --codegen $testfile ./test/book_test_cases/codegen/good$number.s &> $testfile.out
	then
		echo -e "$testfile success ${GREEN}(as expected)${NC} "

		# compile program 
		gcc -g "test/book_test_cases/codegen/good$number.s" src/library/library.c -o "test/book_test_cases/codegen/good$number.out" 
		compile_status=$?

		if [ $compile_status -ne 0 ]; then
			echo "Compilation failed with status $compile_status" >> "$testfile.out"
		fi

		# Run the program
		./test/book_test_cases/codegen/good$number.out &> $testfile.program.out
		run_status=$?
		echo -e "\nexit status $run_status" >> $testfile.program.out

	else
		echo -e "$testfile failure ${RED}(INCORRECT)${NC} "
	fi
done

if ./bin/bminor --codegen ./test/book_test_cases/codegen/bounce.bminor ./test/book_test_cases/codegen/bounce.s &> ./test/book_test_cases/codegen/bounce.bminor.out
then
	echo -e "./test/book_test_cases/codegen/bounce.bminor success ${GREEN}(as expected)${NC}"

	# Paths
	BASE="test/book_test_cases/codegen"
	ASM="$BASE/bounce.s"
	OUT="$BASE/bounce.code.out"

	# Source files
	LIB="src/library/library.c"
	GFX_C="$BASE/gfx.c"

	# Step 2: Compile object files
	gcc -c "$ASM"    -o "$BASE/bounce.o"  -g
	compile_status=$?

	gcc -c "$LIB"    -o "$BASE/library.o" -g
	compile_status=$((compile_status | $?))

	gcc -c "$GFX_C"  -o "$BASE/gfx.o"     -g
	compile_status=$((compile_status | $?))

	# Step 3: Link
	gcc "$BASE/bounce.o" "$BASE/library.o" "$BASE/gfx.o" -o "$OUT" -lX11 -lm
	compile_status=$((compile_status | $?))

	# Write compilation status
	if [ $compile_status -ne 0 ]; then
		echo "Compilation failed with status $compile_status" >> "./test/book_test_cases/codegen/bounce.bminor.out"
	fi

	# Run the program
	"$OUT" &> "$BASE/bounce.bminor.program.out"
	run_status=$?

	echo -e "\nexit status $run_status" >> "$BASE/bounce.bminor.program.out"

else
	echo -e "./test/book_test_cases/codegen/bounce.bminor failure ${RED}(INCORRECT)${NC}"
fi

if ./bin/bminor --codegen ./test/book_test_cases/codegen/maze.bminor ./test/book_test_cases/codegen/maze.s &> ./test/book_test_cases/codegen/maze.bminor.out
then
	echo -e "./test/book_test_cases/codegen/maze.bminor success ${GREEN}(as expected)${NC} "

	# compile program 
	gcc -g "test/book_test_cases/codegen/maze.s" src/library/library.c  -o "test/book_test_cases/codegen/maze.code.out" 
	compile_status=$?

	if [ $compile_status -ne 0 ]; then
		echo "Compilation failed with status $compile_status" >> "./test/book_test_cases/codegen/maze.bminor.out"
	fi

	# Run the program
	./test/book_test_cases/codegen/maze.code.out &> ./test/book_test_cases/codegen/maze.bminor.program.out
	run_status=$?
	echo -e "\nexit status $run_status" >> ./test/book_test_cases/codegen/maze.bminor.program.out

else
	echo -e "./test/book_test_cases/codegen/maze.bminor failure ${RED}(INCORRECT)${NC} "
fi

if ./bin/bminor --codegen ./test/book_test_cases/codegen/fibonnacci.bminor ./test/book_test_cases/codegen/fibonnacci.s &> ./test/book_test_cases/codegen/fibonnacci.bminor.out
then
	echo -e "./test/book_test_cases/codegen/fibonnacci.bminor success ${GREEN}(as expected)${NC} "

	# compile program 
	gcc -g "test/book_test_cases/codegen/fibonnacci.s" src/library/library.c -o "test/book_test_cases/codegen/fibonnacci.code.out" 
	compile_status=$?

	if [ $compile_status -ne 0 ]; then
		echo "Compilation failed with status $compile_status" >> "./test/book_test_cases/codegen/fibonnacci.bminor.out"
	fi

	# Run the program
	./test/book_test_cases/codegen/fibonnacci.code.out &> ./test/book_test_cases/codegen/fibonnacci.bminor.program.out
	run_status=$?
	echo -e "\nexit status $run_status" >> ./test/book_test_cases/codegen/fibonnacci.bminor.program.out

else
	echo -e "./test/book_test_cases/codegen/fibonnacci.bminor failure ${RED}(INCORRECT)${NC} "
fi

