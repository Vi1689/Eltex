APP_NAME = parallel

all:
	gcc -Wall -Wextra -Werror $(APP_NAME)_server.c pthread_pool.c -o $(APP_NAME)_server
	gcc -Wall -Wextra -Werror tcp_client.c -o tcp_client
	gcc -Wall -Wextra -Werror udp_client.c -o udp_client
	clang-format -i *.c 
	clang-format -i *.h 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME)_server.c -o $(APP_NAME)_server

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_NAME)_server

clean:
	rm $(APP_NAME)_server tcp_client udp_client