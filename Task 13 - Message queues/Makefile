APP_NAME_1 = server
APP_NAME_2 = client

all:
	gcc -Wall -Wextra -Werror $(APP_NAME_1).c list.c -o $(APP_NAME_1) -lrt
	gcc -Wall -Wextra -Werror $(APP_NAME_2).c list.c -o $(APP_NAME_2) -lrt -lncursesw
	clang-format -i *.c 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME_1).c list.c -o $(APP_NAME_1) -lrt -lncursesw

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_NAME_1)

clean:
	rm $(APP_NAME_1) $(APP_NAME_2)