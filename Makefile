MIX = mix
CXXFLAGS := --std=c++17

PRIV_DIR = priv
LIB_NAME = $(PRIV_DIR)/casbinex_nif.so

ERLANG_INCLUDE_DIR = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
ERLANG_LIB_DIR =$(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/usr/lib"])])' -s init stop -noshell)

CXXFLAGS += -I$(ERLANG_INCLUDE_DIR) -I$(ERLANG_LIB_DIR) -dynamiclib -Ic_src/include

ifeq ($(wildcard deps/casbinex),)
	CASBINEX_PATH = ../casbinex
else
	CASBINEX_PATH = deps/casbinex
endif

ifneq ($(OS),Windows_NT)
	CXXFLAGS += c_src/casbin_nif.cpp c_src/pg_adapter.cpp c_src/pg_pool.cpp

	ifeq ($(shell uname),Darwin)

		ifeq ($(shell uname -m), arm64)
		  HOMEBREW_PREFIX = /opt/homebrew
          MACHINE_ARCH = arm64
		else
		  HOMEBREW_PREFIX = /usr/local
          MACHINE_ARCH = x86_64
		endif

        CXXFLAGS += c_src/macos/lib/$(MACHINE_ARCH)/casbin.a
		CXXFLAGS += -I$(HOMEBREW_PREFIX)/opt/libpq/include -I$(HOMEBREW_PREFIX)/opt/libpqxx/include -Ic_src/macos/include
		LDFLAGS +=  -L$(HOMEBREW_PREFIX)/opt/libpq/lib -L$(HOMEBREW_PREFIX/opt/libpqxx/lib)
		LDFLAGS += -lpqxx -lpq -flat_namespace -undefined suppress
	else
        CXXFLAGS += -Ic_src/linux/include c_src/linux/lib c_src/linux/lib/libpqxx.a c_src/linux/lib/casbin.a
		LDFLAGS += -lpq
	endif
endif

calling_from_make:
	mix compile

all: $(PRIV_DIR) $(BUILD) $(LIB_NAME)

$(PRIV_DIR):
	mkdir -p $@

$(BUILD):
	/usr/bin/g++ -fPIC -O3 $(CXXFLAGS) -o $@ $^

$(LIB_NAME):
	/usr/bin/g++ -o $@ $(LDFLAGS) $^

clean:
	$(RM) priv/*

.PHONY: all calling_from_make clean
