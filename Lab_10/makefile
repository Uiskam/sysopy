basic_comp :=  gcc -g -Wall -Wextra -Wshadow -Wunused 
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -g -Wall -Wextra -pedantic -Werror

compile:
	$(basic_comp) server.c -o server 
	$(basic_comp) client.c -o client

werr_compile:
	$(werr_comp) server.c -o server 
	$(werr_comp) client.c -o client

mem_compile:
	$(mem_err_comp) server.c -o server 
	$(mem_err_comp) client.c -o client