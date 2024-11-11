APP_NAME = main
APP_PROGRAM = number_1-4

all: $(APP_PROGRAM).o
	gcc -Wall -Wextra -Werror $^ -o $(APP_NAME);
	clang-format -i *.c

$(APP_PROGRAM).o: $(APP_PROGRAM).c
	gcc -c $^ -o $@

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_PROGRAM).c -o $(APP_NAME);

clean:
	rm $(APP_PROGRAM).o $(APP_NAME);

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./$(APP_NAME)

.PHONY: all clean valgrind gdb