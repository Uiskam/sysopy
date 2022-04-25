CC = gcc -Wall -Wextra -pedantic

clean_prod:
	gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address    kons.c   -o kons
	gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address    prod.c   -o prod
	gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address    init.c   -o init

mprod:
	$(CC) -o prod prod.c

mkons:
	$(CC) -o kons kons.c

minit:
	$(CC) -o init init.c

run:  mprod mkons minit
	rm pipe
	mkfifo pipe
	./init 3 pipe 10 1
	

test: mprod mkons minit
	rm pipe
	mkfifo pipe
	./init 3 pipe 10 1
	./checker.sh
	 

	rm pipe
	mkfifo pipe
	./init 1 pipe 10 3
	./checker_1_n.sh
	 
	rm pipe
	mkfifo pipe
	./init 3 pipe 10 3
	./checker.sh

	rm pipe
	mkfifo pipe
	./init 3 pipe 5000 1
	./checker.sh
	 

	rm pipe
	mkfifo pipe
	./init 1 pipe 5000 3
	./checker_1_n.sh
	 

	rm pipe
	mkfifo pipe
	./init 3 pipe 5000 3
	./checker.sh
	 
	
	
