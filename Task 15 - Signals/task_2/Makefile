APP_NAME_1 = first
APP_NAME_2 = second

all:
	gcc -Wall -Wextra -Werror $(APP_NAME_1).c -o $(APP_NAME_1)
	gcc -Wall -Wextra -Werror $(APP_NAME_2).c -o $(APP_NAME_2)
	clang-format -i *.c 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME_1).c -o $(APP_NAME_1)

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_NAME_1)

clean:
	rm $(APP_NAME_1) $(APP_NAME_2)