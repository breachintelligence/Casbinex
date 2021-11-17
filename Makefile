MIX = mix
CFLAGS += -O2 -Wall --std=c++17

ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS += -I$(ERLANG_PATH)

# adjust these as your library desires
CFLAGS += -I/c_src/include
CFLAGS += -lpq

ifeq ($(wildcard deps/casbinex),)
	CASBINEX_PATH = ../casbinex
else
	CASBINEX_PATH = deps/casbinex
endif

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC 
	LDFLAGS += c_src/casbin_nif.cpp c_src/pg_adapter.cpp c_src/pg_pool.cpp

	ifeq ($(shell uname),Darwin)
		CFLAGS += -I/c_src/macos/include -L/c_src/macos/lib -I/usr/local/lib/erlang/usr/include 
		CFLAGS += -I/usr/local/opt/libpq/include -L/usr/local/opt/libpq/lib -L/usr/local/opt/erlang/lib/erlang/lib
		LDFLAGS += c_src/macos/lib/casbin.a -lpqxx -dynamiclib -undefined -dynamic_lookup
	else
		CFLAGS += -I/c_src/linux/include -L/c_src/linux/lib
		LDFLAGS += c_src/linux/lib/libpqxx.a c_src/linux/lib/casbin.a
	endif
endif

.PHONY: all casbinex clean

all: 
	$(MIX) compile 
	
priv/casbinex_nif.so:
	/usr/bin/g++ $(CFLAGS) -shared $(LDFLAGS) -o $@


clean:
	$(MIX) clean
	$(RM) priv/*