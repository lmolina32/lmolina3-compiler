# configurations 

CC=			gcc
CFLAGS=		-Wall -g -std=gnu99
LEX=		flex
LD=			gcc
LDFLAGS=	-L.
YACC=		bison 

TARGETS= bminor

# rules 
all: $(TARGETS)

bminor: bminor.o encoder.o bminor_functions.o scanner.o parser.o tokens_to_string.o 
	$(LD) $(LDFLAGS) -o $@ $^

bminor.o: bminor.c 
	$(CC) $(CFLAGS) -c -o $@ $^

bminor_functions.o: bminor_functions.c bminor_functions.h
	$(CC) $(CFLAGS) -c -o $@ $<

encoder.o: encoder.c encoder.h 
	$(CC) $(CFLAGS) -c -o $@ $<

tokens_to_string.o: tokens_to_string.c tokens_to_string.h
	$(CC) $(CFLAGS) -c -o $@ $<

scanner.c: scanner.flex token.h 
	$(LEX) -o $@ $< 

scanner.o: scanner.c token.h 
	$(CC) -c -o $@ $< -lfl

parser.c token.h: parser.bison 
	$(YACC) -v --defines=token.h --output=parser.c $< 

parser.o: parser.c token.h 
	$(CC) -c -o $@ $<

test:	test-all

test-all: bminor
	@chmod +x ./test/run_all_tests.sh
	@chmod +x ./test/encode/test_encode.sh
	@chmod +x ./test/scanner/test_scanner.sh
	@chmod +x ./test/parser/test_parser.sh
	@./test/run_all_tests.sh

test-encode: bminor
	@echo "Testing Encode"
	@echo "---------------------------------------"
	@chmod +x ./test/encode/test_encode.sh
	@./test/encode/test_encode.sh

test-scanner: bminor
	@echo "Testing Scanner"
	@echo "---------------------------------------"
	@chmod +x ./test/scanner/test_scanner.sh
	@./test/scanner/test_scanner.sh

test-parser: bminor
	@echo "Testing parser"
	@echo "---------------------------------------"
	@chmod +x ./test/parser/test_parser.sh
	@./test/parser/test_parser.sh

clean:
	@echo "Removing Objects and Test Outputs"

	@rm -f $(TARGETS) *.o 
	@rm -f ./test/encode/*.out ./test/scanner/*.out
	@rm -f scanner.c parser.output token.h parser.c
