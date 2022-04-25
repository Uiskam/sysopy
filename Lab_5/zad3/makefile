CC = gcc -Wall -Wextra -pedantic

clean_prod:
	gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address    kons.c   -o kons

mprod:
	$(CC) -o prod prod.c

mkons:
	$(CC) -o kons kons.c

minit:
	$(CC) -o init init.c

run: mprod mkons minit
	rm pipe
	mkfifo pipe
	./init 3 pipe 10 1
	sleep 5
	./checker.sh

test: mprod mkons minit
	rm pipe
	mkfifo pipe
	./init 3 pipe 10 1
	sleep 5
	./checker.sh
	sleep 5

	rm pipe
	mkfifo pipe
	./init 1 pipe 10 3
	sleep 5
	./checker_1_n.sh
	sleep 5

	rm pipe
	mkfifo pipe
	./init 3 pipe 10 3
	sleep 5
	./checker.sh
	sleep 5
	
	rm pipe
	mkfifo pipe
	./init 3 pipe 5000 1
	sleep 5
	./checker.sh
	sleep 5

	rm pipe
	mkfifo pipe
	./init 1 pipe 5000 3
	sleep 5
	./checker_1_n.sh
	sleep 5

	rm pipe
	mkfifo pipe
	./init 3 pipe 5000 3
	sleep 5
	./checker.sh
