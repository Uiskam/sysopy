basic_comp :=  gcc -g -Wall -Wextra -Wshadow -Wunused -O0
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -g -Wall -Wextra -pedantic -Werror

compile:
	$(basic_comp) main.c -o main -lpthread

mem:
	$(mem_err_comp) main.c -o main -lpthread

hand_num: compile
	./main 2 numbers ./resources/hands.ascii.pgm ./results/hands.ascii.inverted.pgm

hand_blk: compile
	./main 3 block ./resources/hands.ascii.pgm ./results/hands.ascii.inverted.pgm

mount_num: compile
	./main 2 numbers ./resources/mountain.ascii.pgm ./results/mountain.ascii.inverted.pgm

mount_blk: compile
	./main 3 block ./resources/mountain.ascii.pgm ./results/mountain.ascii.inverted.pgm

brain_num: compile
	./main 16 numbers ./resources/brain.pgm ./results/brain.inverted.pgm

brain_blk: compile
	./main 1 block ./resources/brain.pgm ./results/brain.inverted.pgm

my_num: compile
	./main 4 numbers ./resources/my.pgm ./results/my_result.pgm

my_blk: compile
	./main 4 block ./resources/my.pgm ./results/my_result.pgm

mem_run: mem
	./main 1 numbers ./resources/mountain.ascii.pgm ./results/moutain.ascii.inverted.pgm


