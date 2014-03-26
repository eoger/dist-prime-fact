all: esclave

esclave: esclave.c brent-pollard.c brent-pollard.h ../shared/common.h
	gcc -O3 -std=gnu99 -o esclave esclave.c brent-pollard.c -lpthread

.PHONY: clean
clean:
	@rm -f *.o esclave 2&> /dev/null
