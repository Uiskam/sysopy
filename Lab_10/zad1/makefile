basic_comp :=  gcc -g -Wall -Wextra -Wshadow -Wunused 
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -g -Wall -Wextra -pedantic -Werror

compile:
	$(basic_comp) server.c -o server -lpthread
	$(basic_comp) client.c -o client

werr_compile:
	$(werr_comp) server.c -o server -lpthread
	$(werr_comp) client.c -o client

mem_compile:
	$(mem_err_comp) server.c -o server -lpthread
	$(mem_err_comp) client.c -o client

srv_compile:
	$(basic_comp) server.c -o server -lpthread

cli_compile:
	$(basic_comp) client.c -o client -lpthread