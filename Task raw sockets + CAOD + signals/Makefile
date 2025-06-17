APP_SERVER = server
APP_CLIENT = client
APP_LIST = my_list

all: docker_create comp_server comp_client
	clang-format -i *.c
	clang-format -i *.h

$(APP_SERVER).o: $(APP_SERVER).c
	gcc -c $^ -o $@

$(APP_LIST).o: $(APP_LIST).c
	gcc -c $^ -o $@

$(APP_CLIENT).o: $(APP_CLIENT).c
	gcc -c $^ -o $@

comp_server: $(APP_SERVER).o $(APP_LIST).o
	gcc -Wall -Wextra -Werror $^ -o $(APP_SERVER)

comp_client: $(APP_CLIENT).o
	gcc -Wall -Wextra -Werror $^ -o $(APP_CLIENT)

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_SERVER).c -o $(APP_SERVER)

docker_create:
	docker build -t raw_docker .

docker_run:
	docker run -it --rm --cap-add=NET_RAW --network my_network raw_docker

valgrind:
	sudo valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_SERVER)

clean: docker_image_rm
	rm $(APP_SERVER) $(APP_CLIENT) *.o

docker_image_rm:
	docker image rm raw_docker:latest	