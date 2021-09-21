#!make
SHELL = /bin/sh

OBJS =  pg_adapter.o
CFLAGS = --std=c++17 -fPIC -Wall
CC = g++
INCLUDES = -I../include
LIBS =  -L../linux/lib ../linux/lib/libpqxx.a ../linux/lib/casbin.a -lpq

all:main

main:$(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o test $(OBJS) main.cpp $(LIBS)

pg_adapter.o: ../pg_adapter.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c ../pg_adapter.cpp

clean:
	-rm -f *.o core *.core

