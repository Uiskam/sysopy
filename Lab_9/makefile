basic_comp :=  gcc -g -Wall -Wextra -Wshadow -Wunused -O0
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -g -Wall -Wextra -pedantic -Werror

compile:
	$(basic_comp) main.c -o main -lpthread

mem:
	$(mem_err_comp) main.c -o main -lpthread

run: compile
	./main

mem_run: compile
	./main