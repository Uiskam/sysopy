basic_comp :=  gcc -Wall -Wextra -pedantic
mem_err_comp := gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address    main.c   -o zad2
werr_comp := gcc -Wall -Wextra -pedantic -Werror main.c -o zad2

clean:
	rm zad1 2> /dev/null

compile: clean
	$(basic_comp) main.c -o zad1
	./zad1

mem: clean
	$(mem_err_comp) main.c -o zad1
	./zad1

werr: clean
	$(werr_comp) main.c -o zad1
	./zad1
