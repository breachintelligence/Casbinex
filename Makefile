MIX = mix
CXXFLAGS := --std=c++17

PRIV_DIR = ${MIX_APP_PATH}/priv
LIB_NAME = $(PRIV_DIR)/casbinex_nif.so

INCLUDES += -I${ERL_EI_INCLUDE_DIR} -Ic_src/include
LIBS += -L${ERL_EI_LIBDIR} 
SOURCES += c_src/casbin_nif.cpp c_src/pg_adapter.cpp c_src/pg_pool.cpp
STATIC_LIBS = 

ifneq ($(OS),Windows_NT)
	ifeq ($(shell uname),Darwin)
		# is mac
		HOMEBREW_PREFIX = ${HOMEBREW_REPOSITORY}
		MACHINE_ARCH = x86_64

		ifeq ($(shell uname -m), arm64)
       	  		MACHINE_ARCH = arm64
		endif

		INCLUDES +=  -I$(HOMEBREW_PREFIX)/opt/libpq/include -I$(HOMEBREW_PREFIX)/opt/libpqxx/include -Ic_src/macos/include
		LIBS += -L$(HOMEBREW_PREFIX)/opt/libpq/lib -L$(HOMEBREW_PREFIX)/opt/libpqxx/lib -L${ERL_EI_LIBDIR} -lpqxx -lpqxx
		STATIC_LIBS += c_src/macos/lib/$(MACHINE_ARCH)/casbin.a
		CXXFLAGS += -flat_namespace -undefined suppress -dynamiclib 

	else

	# Is Linux 
		CXXFLAGS += -shared
		INCLUDES += -Ic_src/linux/include
		LIBS += -Lc_src/linux/lib 
		STATIC_LIBS += c_src/linux/lib/libpqxx.a c_src/linux/lib/casbin.a -lpq

	endif
endif

calling_from_make:
	mix compile

all: $(PRIV_DIR) $(LIB_NAME)

$(PRIV_DIR):
	mkdir -p $@

$(LIB_NAME):
	/usr/bin/g++ -fPIC -O3 $(CXXFLAGS)  $(INCLUDES)  $(LIBS) -o $@  $(SOURCES)  $(STATIC_LIBS)

clean:
	$(RM) $(LIB_NAME)

.PHONY: all calling_from_make clean
