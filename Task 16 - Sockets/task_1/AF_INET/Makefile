APP_NAME = tcp

all:
	gcc -Wall -Wextra -Werror $(APP_NAME)_server.c -o $(APP_NAME)_server
	gcc -Wall -Wextra -Werror $(APP_NAME)_client.c -o $(APP_NAME)_client
	clang-format -i *.c 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME)_server.c -o $(APP_NAME)_server

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_NAME)_server

clean:
	rm $(APP_NAME)_server $(APP_NAME)_client