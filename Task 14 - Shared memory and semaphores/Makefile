APP_NAME_SERVER=server
APP_NAME_CLIENT=client

all:
	gcc -Wall -Wextra -Werror $(APP_NAME_CLIENT).c list.c -o $(APP_NAME_CLIENT) -lrt -lpthread -lncursesw
	gcc -Wall -Wextra -Werror $(APP_NAME_SERVER).c list.c -o $(APP_NAME_SERVER) -lrt -lpthread
	clang-format -i *.c 
	clang-format -i *.h 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME_SERVER).c list.c -o $(APP_NAME_SERVER) -lrt -lncursesw

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_NAME_SERVER)

clean:
	rm $(APP_NAME_SERVER) $(APP_NAME_CLIENT)