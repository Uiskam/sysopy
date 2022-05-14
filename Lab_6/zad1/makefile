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
	sleep 0.5
	./client 1 < instructions/client_1_ins &
	./client 2 < instructions/client_2_ins &
	./client 3 < instructions/client_3_ins &
	./client 4 < instructions/client_4_ins &

min_run: compile_cli compile_srv clear_queues
	./server &
	./client 1 < instructions/client_1_ins &
