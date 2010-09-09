



all: main.c interfaces.c error.c wrappers.c
	gcc -Wall -o status main.c interfaces.c error.c wrappers.c

clean:
	rm -rf status