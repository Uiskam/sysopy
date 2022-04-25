CC = gcc -Wall -Wextra -pedantic

clean_prod:
	gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address    kons.c   -o kons

mprod:
	rm pipe
	mkfifo pipe
	$(CC) -o prod prod.c
	./prod pipe 10 supply.txt 10

mkons:
	$(CC) -o kons kons.c

minit:
	$(CC) -o init init.c

run:
	rm pipe
	mkfifo pipe
	$(CC) -o prod prod.c
	$(CC) -o kons kons.c
	$(CC) -o init init.c
	./init 5 pipe 10