compile: 
	gcc -Wall -Wextra -pedantic main.c -o zad2

clean:
	gcc -Wall -Wextra -Wshadow -Wunused -Wconversion -g -O2 -fsanitize=address    main.c   -o zad2

data: compile
	./zad2 data

sender: compile
	./zad2 nadawca

send:
	gcc -Wall -Wextra -pedantic main.c -o zad2
	./zad2 example@egxample.com topic "tekst to be send"

pedantic:
	gcc -Wall -Wextra -pedantic -Werror main.c -o zad2
