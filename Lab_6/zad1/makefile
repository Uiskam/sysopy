basic_comp :=  gcc -Wall -Wextra -pedantic
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -Wall -Wextra -pedantic -Werror


clear_queues:
	ipcrm --all=msg

compile_cli:
	$(basic_comp) client.c -o client

mem_cli:
	$(mem_err_comp) client.c -o client

compile_srv:
	$(basic_comp) server.c -o server

mem_srv:
	$(mem_err_comp) server.c -o server

run_test: compile_cli compile_srv clear_queues
	./server &
	./client 1 < client_1_ins

