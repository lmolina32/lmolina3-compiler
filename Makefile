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
				$(wildcard src/utils/*.h) \
				$(wildcard build/*.h) 

INCLUDES=		-Isrc/main \
				-Isrc/encoder \
				-Isrc/scanner \
				-Isrc/parser \
				-Isrc/ast \
				-Isrc/library \
				-Isrc/utils \
				-Ibuild

OBJECTS=		build/bminor.o \
				build/bminor_functions.o \
				build/encoder.o \
				build/tokens_to_string.o \
				build/scanner.o \
				build/parser.o \
				build/decl.o \
				build/expr.o \
				build/param_list.o \
				build/stmt.o \
				build/type.o \
				build/symbol.o 

BMINOR=			bin/bminor 
OLD_BMINOR=		bminor

# Rules 

all: dirs $(BMINOR) $(OLD_BMINOR)

dirs:
	@echo "making bin and build directories"
	@mkdir -p bin
	@mkdir -p build

# Compile bminor 
$(OLD_BMINOR): $(OBJECTS)
	@echo "Linking $@"
	@$(LD) $(LDFLAGS) $(INCLUDES) -o $@ $^

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
	
# Compile ast 
build/%.o: src/ast/%.c $(HEADERS)
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
build/parser.o: build/parser.c build/token.h $(HEADERS)
	@echo "Compiling $@"
	@$(CC) $(INCLUDES) -c -o $@ $<

# testing 

test:	test-all

test-all: $(BMINOR) 
	@chmod +x ./test/scripts/*.sh
	@./test/scripts/run_all_tests.sh

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
	
test-printer: $(BMINOR) 
	@echo "Testing printer"
	@echo "---------------------------------------"
	@chmod +x ./test/scripts/test_printer.sh
	@./test/scripts/test_printer.sh

test-book: $(BMINOR)
	@chmod +x ./test/scripts/run_book_tests.sh
	@chmod +x ./test/book_test_cases/scripts/*.sh
	@./test/scripts/run_book_tests.sh

# clean 

clean:
	@echo "Removing Objects"
	@rm -f $(OBJECTS)

	@echo "Removing Generated Files"
	@rm -f ./build/scanner.c ./build/parser.c ./build/token.h ./build/parser.output 

	@echo "Removing Test Logs"
	@rm -f ./test/encode/*.out ./test/scanner/*.out ./test/parser/*.out ./test/printer/*.out
	@rm -f ./test/book_test_cases/parser/*.out ./test/book_test_cases/printer/*.out

	@echo "Removing bminor"
	@rm -f $(BMINOR) $(OLD_BMINOR)

