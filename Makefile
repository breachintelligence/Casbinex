MIX = mix
CXXFLAGS := --std=c++17

MIX_APP_PATH ?= .
PRIV_DIR = ${MIX_APP_PATH}/priv
LIB_NAME = $(PRIV_DIR)/casbinex_nif.so
DEPS_DIR = c_src/_deps
CASBIN_LIB = $(DEPS_DIR)/casbin/casbin.a
CASBIN_VERSION = 1.44.0

INCLUDES += -I${ERL_EI_INCLUDE_DIR} -Ic_src/include -I$(DEPS_DIR)/include
LIBS += -L${ERL_EI_LIBDIR} 
SOURCES += c_src/casbin_nif.cpp c_src/pg_adapter.cpp c_src/pg_pool.cpp
STATIC_LIBS = $(CASBIN_LIB)

ifneq ($(OS),Windows_NT)
	ifeq ($(shell uname),Darwin)
		# is mac
		HOMEBREW_PREFIX = ${HOMEBREW_REPOSITORY}
		MACHINE_ARCH = x64
		KERNEL_NAME = darwin

		ifeq ($(shell uname -m), arm64)
			MACHINE_ARCH = arm64
		endif

		INCLUDES +=  -I$(HOMEBREW_PREFIX)/opt/libpq/include -I$(HOMEBREW_PREFIX)/opt/libpqxx/include -Ic_src/macos/include
		LIBS += -L$(HOMEBREW_PREFIX)/opt/libpq/lib -L$(HOMEBREW_PREFIX)/opt/libpqxx/lib -L${ERL_EI_LIBDIR} -lpqxx -lpqxx
		CXXFLAGS += -flat_namespace -undefined suppress -dynamiclib 

	else

	# Is Linux 
		KERNEL_NAME = linux
		CXXFLAGS += -shared

		ifeq ($(shell uname -m), aarch64)
			MACHINE_ARCH = arm64
			INCLUDES += -Ic_src/linux/arm64/include
			LIBS += -Lc_src/linux/arm64/lib 
			STATIC_LIBS += c_src/linux/arm64/lib/libpqxx.a -lpq
		else
			MACHINE_ARCH = x64
			INCLUDES += -Ic_src/linux/x64/include
			LIBS += -Lc_src/linux/x64/lib 
			STATIC_LIBS += c_src/linux/x64/lib/libpqxx.a -lpq
		endif

	endif
endif


calling_from_make:
	mix compile

all: $(DEPS_DIR) $(PRIV_DIR) $(LIB_NAME)

$(DEPS_DIR):
	mkdir $(DEPS_DIR)
	curl -so $(DEPS_DIR)/casbin.tgz https://dl.polarity.io/2de1d0f2ddd422cc924be2f3a84e9668/casbin/$(CASBIN_VERSION)/casbin-$(CASBIN_VERSION)-$(KERNEL_NAME)-$(MACHINE_ARCH).tgz
	tar zxf $(DEPS_DIR)/casbin.tgz -C c_src/_deps
	rm -f $(DEPS_DIR)/casbin.tgz

$(PRIV_DIR):
	mkdir -p $@

$(LIB_NAME): $(DEPS_DIR)
	/usr/bin/g++ -fPIC -O3 $(CXXFLAGS)  $(INCLUDES)  $(LIBS) -o $@  $(SOURCES)  $(STATIC_LIBS)

clean:
	$(RM) $(LIB_NAME)
	$(RM) -rf $(DEPS_DIR)

.PHONY: all calling_from_make clean
