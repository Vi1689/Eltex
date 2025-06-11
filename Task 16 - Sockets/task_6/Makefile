APP_SERVER = udp
APP_CLIENT = raw

all: docker_create
	gcc -Wall -Wextra -Werror $(APP_SERVER)_server.c -o $(APP_SERVER)_server
	gcc -Wall -Wextra -Werror $(APP_CLIENT)_client.c -o $(APP_CLIENT)_client
	clang-format -i *.c 

gdb:
	gcc -g -Wall -Wextra -Werror $(APP_CLIENT)_client.c -o $(APP_CLIENT)_client

docker_create:
	docker build -t raw_docker .

docker_run:
	docker run -it --rm --cap-add=NET_RAW --network my_network raw_docker

valgrind:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./$(APP_SERVER)_server

clean: docker_image_rm
	rm $(APP_SERVER)_server $(APP_CLIENT)_client

docker_image_rm:
	docker image rm raw_docker:latest