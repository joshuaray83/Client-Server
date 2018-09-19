all: server.exe client.exe

server.exe: server.c server.o
	gcc -Wall -ansi -pedantic -c server.c
	gcc -o server.exe server.o

client.exe: client.c client.o
	gcc -Wall -ansi -pedantic -c client.c
	gcc -o client.exe client.o
