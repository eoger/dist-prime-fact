all: maitre

maitre: maitre.c ../shared/common.h
	gcc -O3 -std=gnu99 -o maitre maitre.c -lpthread

.PHONY: clean
clean:
	@rm -f *.o maitre 2&> /dev/null
