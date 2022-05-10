basic_comp :=  gcc -Wall -Wextra -pedantic
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -Wall -Wextra -pedantic -Werror

clean:
	rm server 2> /dev/null
	rm client 2> /dev/null

compile_cli:
	$(basic_comp) client.c -o client

mem_cli:
	$(mem_err_comp) client.c -o client

compile_srv:
	$(basic_comp) server.c -o server

mem_srv:
	$(mem_err_comp) server.c -o server

mem: clean
	$(mem_err_comp) client.c -o client

werr: clean
	$(werr_comp) client.c -o client
