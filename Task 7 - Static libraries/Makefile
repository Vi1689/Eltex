APP_NAME = main
LIB_NAME = libcalc
NOT_LIB_NAME = calc

all: $(LIB_NAME).a $(APP_NAME).o
	gcc -Wall -Wextra -Werror $^ -o $(APP_NAME) -L. -l$(NOT_LIB_NAME)
	clang-format -i *.c
	clang-format -i *.h

$(APP_NAME).o: $(APP_NAME).c
	gcc -c $^ -o $@

$(LIB_NAME).a: add.o sub.o mul.o div.o
	ar rc $@ $^

add.o: add.c
	gcc -c $^ -o $@

sub.o: sub.c
	gcc -c $^ -o $@

mul.o: mul.c
	gcc -c $^ -o $@

div.o: div.c
	gcc -c $^ -o $@

clean:
	rm $(APP_NAME) *.o *.a

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./$(APP_NAME)

.PHONY: all clean valgrind