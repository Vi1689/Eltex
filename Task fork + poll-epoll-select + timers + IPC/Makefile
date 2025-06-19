APP_NAME = main

all:
	gcc -Wall -Wextra -Werror $(APP_NAME).c -o $(APP_NAME)
	clang-format -i *.c 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME).c -o $(APP_NAME)

valgrind:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(APP_NAME)

clean:
	rm $(APP_NAME)