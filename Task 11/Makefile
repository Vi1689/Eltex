APP_NAME = task_3

all: $(APP_NAME).o
	gcc -Wall -Wextra -Werror $^ -o $(APP_NAME)
	clang-format -i *.c 

$(APP_NAME).o: $(APP_NAME).c
	gcc -c $^ -o $@ -lpthread

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME).c -o $(APP_NAME)

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_NAME)

clean:
	rm *.o $(APP_NAME)