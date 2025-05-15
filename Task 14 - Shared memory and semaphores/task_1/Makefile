APP_NAME_SERVER=task_1_server
APP_NAME_CLIENT=task_1_client

all:
	gcc -Wall -Wextra -Werror $(APP_NAME_CLIENT).c -o $(APP_NAME_CLIENT) -lrt
	gcc -Wall -Wextra -Werror $(APP_NAME_SERVER).c -o $(APP_NAME_SERVER) -lrt
	clang-format -i *.c 
	clang-format -i *.h 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME_SERVER).c list.c -o $(APP_NAME_SERVER) -lrt -lncursesw

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_NAME_SERVER)

clean:
	rm $(APP_NAME_SERVER) $(APP_NAME_CLIENT)