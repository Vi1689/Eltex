APP_NAME = main
LIB_NAME = libcalc
NOT_LIB_NAME = calc

all: $(LIB_NAME).so $(APP_NAME).o
	gcc -Wall -Wextra -Werror $(APP_NAME).o -o $(APP_NAME) -L. -l$(NOT_LIB_NAME)
	clang-format -i *.c
	clang-format -i *.h

$(APP_NAME).o: $(APP_NAME).c
	gcc -c $^ -o $@

$(LIB_NAME).so: add.o sub.o mul.o div.o
	gcc -shared -o $@ $^

add.o: add.c
	gcc -c -fPIC $^ -o $@

sub.o: sub.c
	gcc -c -fPIC $^ -o $@

mul.o: mul.c
	gcc -c -fPIC $^ -o $@

div.o: div.c
	gcc -c -fPIC $^ -o $@

clean:
	rm $(APP_NAME) *.o *.so

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./$(APP_NAME)

.PHONY: all clean valgrind