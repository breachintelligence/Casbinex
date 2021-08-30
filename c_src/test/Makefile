#!make
SHELL = /bin/sh

OBJS =  pg_adapter.o
CFLAGS = --std=c++17 -fPIC -Wall
CC = g++
INCLUDES = -I../include
LIBS =  -L../lib/linux ../lib/linux/libpqxx.a ../lib/linux/casbin.a -lpq

all:main

main:$(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o test $(OBJS) main.cpp $(LIBS)

pg_adapter.o: ../pg_adapter.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c ../pg_adapter.cpp $(LIBS)

clean:
	-rm -f *.o core *.core

