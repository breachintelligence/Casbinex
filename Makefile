MIX = mix
CXXFLAGS += --std=c++17

PRIV = priv

ERLANG_INCLUDE_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
ERLANG_LIB_PATH =$(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/lib"])])' -s init stop -noshell)

CXXFLAGS += -Ic_src/include -I$(ERLANG_INCLUDE_PATH)

ifeq ($(wildcard deps/casbinex),)
	CASBINEX_PATH = ../casbinex
else
	CASBINEX_PATH = deps/casbinex
endif

ifneq ($(OS),Windows_NT)
	LDFLAGS += c_src/casbin_nif.cpp c_src/pg_adapter.cpp c_src/pg_pool.cpp

	ifeq ($(shell uname),Darwin)

		ifeq ($(shell uname -m), arm64)
		  HOMEBREW_PREFIX = /opt/homebrew
		else
		  HOMEBREW_PREFIX = /usr/local
		endif

		CXXFLAGS += -Ic_src/macos/include -I$(ERLANG_INCLUDE_PATH) -L$(HOMEBREW_PREFIX)/opt/libpq/lib
		CXXFLAGS += -fPIC -O3 -Lc_src/macos/lib -L$(HOMEBREW_PREFIX)/opt/libpq/lib -L$(ERLANG_LIB_PATH) -dynamiclib
		LDFLAGS += c_src/macos/lib/casbin.a -lpqxx -lpq -flat_namespace -undefined suppress
	else
		CXXFLAGS += -Ic_src/linux/include -Lc_src/linux/lib -fPIC -O3
		LDFLAGS += c_src/linux/lib/libpqxx.a c_src/linux/lib/casbin.a -lpq
	endif
endif

.PHONY: all casbinex clean

all:
	[ -d $(PRIV) ] || mkdir -p $(PRIV)
	$(MIX) compile

priv/casbinex_nif.so:
	/usr/bin/g++ $(CXXFLAGS) -shared -o $@ $(LDFLAGS)

clean:
	$(MIX) clean
	$(RM) priv/*
