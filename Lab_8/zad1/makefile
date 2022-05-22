basic_comp :=  gcc -Wall -Wextra -pedantic -Wshadow -Wunused
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address
werr_comp := gcc -Wall -Wextra -pedantic -Werror

compile_main:
	$(basic_comp) main.c -o main

mem_main:
	$(mem_err_comp) main.c -o main

compile_prod:
	$(basic_comp) producer.c -o producer

mem_prod:
	$(mem_err_comp) producer.c -o producer

compile_del:
	$(basic_comp) deliver.c -o deliver

mem_del:
	$(mem_err_comp) deliver.c -o deliver

all: compile_del compile_main compile_prod

compile: all

run:
	./main

