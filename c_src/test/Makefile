MIX = mix
CXXFLAGS := -std=c++17 -fPIC -Wall

MIX_APP_PATH ?= .
DEPS_DIR = ../_deps
SRC_DIRS := $(shell realpath ../)
CASBIN_LIB = $(DEPS_DIR)/casbin/casbin.a
BUILD_DIR=build

INC_DIRS += $(DEPS_DIR)/include ../

# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

SOURCES := $(shell find $(SRC_DIRS)  -maxdepth 1 ! -name '*_nif.cpp' -name '*.cpp' -or -name '*.c' -or -name '*.s' )
OBJS := $(SOURCES:%=$(BUILD_DIR)/%.o)

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

		ifeq ($(shell uname -m), aarch64)
			MACHINE_ARCH = arm64
			INCLUDES += -I../linux/arm64/include
			LIBS += -L../linux/arm64/lib 
			STATIC_LIBS += ../linux/arm64/lib/libpqxx.a -lpq
		else
			MACHINE_ARCH = x64
			INCLUDES += -I../linux/x64/include
			LIBS += -L../linux/x64/lib 
			STATIC_LIBS += ../linux/x64/lib/libpqxx.a -lpq
		endif

	endif
endif


CPPFLAGS := $(INC_FLAGS) $(INCLUDES)

all: test

./test: $(OBJS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(CXXFLAGS) -o test $(OBJS) main.cpp $(LIBS) $(STATIC_LIBS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


clean:
	-rm -f *.o core *.core ../*.o ../*.d *.d
	-rm -rf build

