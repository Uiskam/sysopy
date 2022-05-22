basic_comp :=  gcc -g -Wall -Wextra -Wshadow -Wunused -O0
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -g -Wall -Wextra -pedantic -Werror

compile:
	$(basic_comp) main.c -o main

mem:
	$(mem_err_comp) main.c -o main

run: compile
	./main 1 numbers ./resources/mountain.ascii.pgm ./results/moutain.ascii.inverted.pgm

my: compile
	./main 1 numbers ./resources/my.pgm ./results/moutain.ascii.inverted.pgm

mem_run: mem
	./main 1 numbers ./resources/mountain.ascii.pgm ./results/moutain.ascii.inverted.pgm


