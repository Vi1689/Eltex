APP_NAME = main
LIB_NAME = my_list

all: $(APP_NAME).o $(LIB_NAME).o
	gcc  -Wall -Wextra -Werror $^ -o $(APP_NAME)
	clang-format -i *.c
	clang-format -i *.h

$(LIB_NAME).o: $(LIB_NAME).c
	gcc -c $^ -o $@

$(APP_NAME).o: $(APP_NAME).c
	gcc -c $^ -o $@

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_NAME).c $(LIB_NAME).c -o $(APP_NAME)

clean:
	rm $(APP_NAME).o $(LIB_NAME).o $(APP_NAME)

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./$(APP_NAME)

.PHONY: all clean valgrind gdb