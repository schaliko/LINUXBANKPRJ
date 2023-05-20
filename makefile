# Compiler
CC = g++

all: init client deinit

init :main_init.o bank.o bank.h
	g++ main_init.o bank.o bank.h -o init

client: main_client.o bank.o bank.h
	g++ main_client.o bank.o bank.h -o client

deinit: main_deinit.o bank.o bank.h
	g++ main_deinit.o bank.o bank.h -o deinit


clean:
	rm -f *.o client init deinit