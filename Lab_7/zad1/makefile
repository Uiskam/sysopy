basic_comp :=  gcc -Wall -Wextra -pedantic -Wshadow -Wunused
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -Wall -Wextra -pedantic -Werror

compile_main:
	$(basic_comp) main.c -o main

mem_main:
	$(mem_err_comp) main.c -o main