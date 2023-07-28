test:
	gcc -Wall -pedantic -std=c89 src/test.c src/map.c src/lfb.c -o test
	./test
clean:
	rm test
