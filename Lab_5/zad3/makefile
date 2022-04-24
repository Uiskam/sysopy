CC = gcc -Wall -Wextra -pedantic

clean_prod:
	gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address    kons.c   -o kons

mprod:
	$(CC) -o prod prod.c
	./prod pipe 1 supply.txt 30

mkons:
	$(CC) -o kons kons.c

minit:
	$(CC) -o init init.c