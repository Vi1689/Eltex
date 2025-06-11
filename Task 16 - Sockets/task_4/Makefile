APP_SERVER = udp
APP_CLIENT = raw

all:
	gcc -Wall -Wextra -Werror $(APP_SERVER)_server.c -o $(APP_SERVER)_server
	gcc -Wall -Wextra -Werror $(APP_CLIENT)_client.c -o $(APP_CLIENT)_client
	clang-format -i *.c 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_CLIENT)_client.c -o $(APP_CLIENT)_client

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_SERVER)_server

clean:
	rm $(APP_SERVER)_server $(APP_CLIENT)_client