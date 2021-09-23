#!make
SHELL = /bin/sh

OBJS =  pg_adapter.o
CFLAGS = -std=c++17 -fPIC -Wall
CC = g++
INCLUDES = -I../include

ifeq ($(OS),Windows_NT)
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
			INCLUDES += -I../linux/include
			LIBS =  -L../linux/lib ../linux/lib/casbin.a ../linux/lib/libpqxx.a -lpq
    endif
    ifeq ($(UNAME_S),Darwin)
			LIBS = -L/usr/local/opt/libpq/lib -L/usr/local/opt/libpqxx/lib -L/usr/local/opt/erlang/lib/erlang/lib  -L../macos/lib ../macos/lib/casbin.a -lpq -lpqxx
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)

    endif
    ifneq ($(filter %86,$(UNAME_P)),)

    endif
    ifneq ($(filter arm%,$(UNAME_P)),)

    endif
endif



all:main

main:$(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o test $(OBJS) main.cpp $(LIBS)

pg_adapter.o: ../pg_adapter.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c ../pg_adapter.cpp

clean:
	-rm -f *.o core *.core

