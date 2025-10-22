# Configuration 

CC=			gcc
LD=			gcc
CFLAGS=		-Wall -Wextra -g -std=gnu99 -Og
LDFLAGS=	-Lbuild 

YACC=		bison 
LEX=		flex

# Variables

HEADERS=		$(wildcard src/main/*.h) \
				$(wildcard src/encoder/*.h) \
				$(wildcard src/scanner/*.h) \
				$(wildcard src/parser/*.h) \
				$(wildcard src/ast/*.h) \
				$(wildcard src/library/*.h) \
				$(wildcard build/*.h) 

INCLUDES=		-Isrc/main \
				-Isrc/encoder \
				-Isrc/scanner \
				-Isrc/parser \
				-Isrc/ast \
				-Isrc/library \
				-Ibuild

SOURCES= 		src/main/bminor.c \
				src/main/bminor_functions.c \
				src/encoder/encoder.c \
				src/scanner/tokens_to_string.c \

OBJECTS=		build/bminor.o \
				build/bminor_functions.o \
				build/encoder.o \
				build/tokens_to_string.o \
				build/scanner.o \
				build/parser.o

BMINOR=			bin/bminor 

# Rules 

all: $(BMINOR)

# Compile bminor 
$(BMINOR): $(OBJECTS)
	@echo "Linking $@"
	@$(LD) $(LDFLAGS) $(INCLUDES) -o $@ $^

build/%.o: src/main/%.c $(HEADERS) build/token.h 
	@echo "Compiling $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# Compile encoder 
build/%.o: src/encoder/%.c $(HEADERS)
	@echo "Compiling $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# Compile tokens_to_string
build/%.o: src/scanner/%.c $(HEADERS)
	@echo "Compiling $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
	
# Generate scanner
build/scanner.c: src/scanner/scanner.flex build/token.h 
	@echo "Generating $@"
	@$(LEX) -o $@ $<

# Generate parser
build/parser.c build/token.h: src/parser/parser.bison 
	@echo "Generating $@"
	@$(YACC) -v --defines=build/token.h --output=build/parser.c $<

# compile scanner 
build/scanner.o: build/scanner.c build/token.h
	@echo "Compiling $@"
	@$(CC) $(INCLUDES) -c -o $@ $< -lfl 

# compile parser 
build/parser.o: build/parser.c build/token.h
	@echo "Compiling $@"
	@$(CC) $(INCLUDES) -c -o $@ $<

# testing 

test:	test-all

test-all: $(BMINOR) 
	@chmod +x ./test/run_all_tests.sh
	@chmod +x ./test/run_book_tests.sh
	@chmod +x ./test/scripts/*.sh
	@./test/run_all_tests.sh

test-encode: $(BMINOR) 
	@echo "Testing Encode"
	@echo "---------------------------------------"
	@chmod +x ./test/scripts/test_encode.sh
	@./test/scripts/test_encode.sh

test-scanner: $(BMINOR) 
	@echo "Testing Scanner"
	@echo "---------------------------------------"
	@chmod +x ./test/scripts/test_scanner.sh
	@./test/scripts/test_scanner.sh

test-parser: $(BMINOR) 
	@echo "Testing parser"
	@echo "---------------------------------------"
	@chmod +x ./test/scripts/test_parser.sh
	@./test/scripts/test_parser.sh

# clean 

clean:
	@echo "Removing Objects"
	@rm -f $(OBJECTS)

	@echo "Removing Generated Files"
	@rm -f ./build/scanner.c ./build/parser.c ./build/token.h ./build/parser.output 

	@echo "Removing Test Logs"
	@rm -f ./test/encode/*.out ./test/scanner/*.out ./test/parser/*.out
	@rm -f ./test/book_test_cases/parser/*.out

	@echo "Removing bminor"
	@rm -f $(BMINOR)

