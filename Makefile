all: esclave maitre

esclave: esclave.c brent-pollard.c brent-pollard.h common.h
	gcc -O3 -std=gnu99 -o esclave esclave.c brent-pollard.c -lpthread

maitre: maitre.c common.h
	gcc -O3 -std=gnu99 -o maitre maitre.c -lpthread

.PHONY: clean
clean:
	@rm -f *.o maitre esclave 2&> /dev/null
