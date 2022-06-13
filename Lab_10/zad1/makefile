basic_comp :=  gcc -g -Wall -Wextra -Wshadow -Wunused -O0 -pthread
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address -pthread
werr_comp := gcc -g -Wall -Wextra -pedantic -Werror -pthread
valgrind_comp :=  gcc -o executable -Wall -ggdb3 -pthread

compile:
	$(basic_comp) -o server server.c
	$(basic_comp) -o client client.c

mem:
	$(mem_err_comp) -o server server.c
	$(mem_err_comp) -o client client.c

werr:
	$(werr_comp) -o server server.c
	$(werr_comp) -o client client.c

valgrind:
	$(valgrind_comp) -o server server.c
	$(valgrind_comp) -o client client.c

val_run: valgrind
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./server 7001 socket

mem_srv:
	$(mem_err_comp) -o server server.c

mem_cli:
	$(mem_err_comp) -o client client.c
	
mem_run: mem
	./server 7001 socket

run: compile
	./server 7011 socket