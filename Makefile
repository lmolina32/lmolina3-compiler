CC=		gcc
CFLAGS=		-Wall -g -std=gnu99
LD=		gcc
LDFLAGS=	-L.

TARGETS= bminor

all: $(TARGETS)

encoder.o: encoder.c 
	$(CC) $(CFLAGS) -c -o $@ $^

bminor.o: bminor.c 
	$(CC) $(CFLAGS) -c -o $@ $^

bminor: bminor.o encoder.o
	$(LD) $(LDFLAGS) -o $@ $^

test-encoder: bminor
	@chmod +x runtest.sh
	@./runtest.sh
	
test:	test-encoder

clean:
	@rm -f $(TARGETS) *.o 
	@rm -f ./test/encode/*.out
