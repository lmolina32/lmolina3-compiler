# configurations 

CC=			gcc
CFLAGS=		-Wall -g -std=gnu99
LEX=		flex
LD=			gcc
LDFLAGS=	-L.

TARGETS= bminor

# rules 
all: $(TARGETS)


bminor: bminor.o encoder.o bminor_functions.o scanner.o
	$(LD) $(LDFLAGS) -o $@ $^

bminor.o: bminor.c 
	$(CC) $(CFLAGS) -c -o $@ $^

bminor_functions.o: bminor_functions.c bminor_functions.h
	$(CC) $(CFLAGS) -c -o $@ $<

encoder.o: encoder.c encoder.h 
	$(CC) $(CFLAGS) -c -o $@ $<

scanner.c: scanner.flex
	$(LEX) -o $@ $< 

scanner.o: scanner.c 
	$(CC) -c -o $@ $< -lfl

test:	test-all

test-all: bminor
	@chmod +x ./test/run_all_tests.sh
	@chmod +x ./test/encode/test_encode.sh
	@chmod +x ./test/scanner/test_scanner.sh
	@./test/run_all_tests.sh

clean:
	@echo "Removing Objects and Test Outputs"

	@rm -f $(TARGETS) *.o 
	@rm -f ./test/encode/*.out
	@rm -f scanner.c
