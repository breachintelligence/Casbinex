MIX = mix
CFLAGS += --std=c++17

ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS += -Ic_src/include -I$(ERLANG_PATH)

ifeq ($(wildcard deps/casbinex),)
	CASBINEX_PATH = ../casbinex
else
	CASBINEX_PATH = deps/casbinex
endif

ifneq ($(OS),Windows_NT)
	LDFLAGS += c_src/casbin_nif.cpp c_src/pg_adapter.cpp c_src/pg_pool.cpp

	ifeq ($(shell uname),Darwin)
		CFLAGS += -Ic_src/macos/include -I/usr/local/lib/erlang/usr/include/ -L/usr/local/opt/libpq/lib
		CFLAGS += -fPIC -O3 -Lc_src/macos/lib -L/usr/local/opt/libpq/lib -L/usr/local/opt/erlang/lib/erlang/lib -dynamiclib
		LDFLAGS += c_src/macos/lib/casbin.a -lpqxx -lpq -flat_namespace -undefined suppress
	else
		CFLAGS += -Ic_src/linux/include -Lc_src/linux/lib -fPIC -O3
		LDFLAGS += c_src/linux/lib/libpqxx.a c_src/linux/lib/casbin.a -lpq
	endif
endif

.PHONY: all casbinex clean

all: 
	$(MIX) compile 

priv/casbinex_nif.so:
	/usr/bin/g++ $(CFLAGS) -shared -o $@ $(LDFLAGS)

clean:
	$(MIX) clean
	$(RM) priv/*