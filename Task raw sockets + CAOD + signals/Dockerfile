FROM alpine:latest

RUN apk update && \
    apk add --no-cache \
    gcc \
    musl-dev \
    make \
    git \
    nano \
    linux-headers \
    bash  

WORKDIR /app

COPY client.c /app/

RUN gcc -Wall -Wextra -Werror client.c -o main

CMD ["sh"]