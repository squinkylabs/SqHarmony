# If build rack yourself, it will be two levels up from us.
# If you are using the rack SDK and put is somewhere different,
# point to it here.
RACK_DIR ?= ../..

# Specify extra directories to search for include files.
FLAGS += -I./dsp
FLAGS += -I./composites
FLAGS += -I./util/dsp
FLAGS += -I./util/quant
FLAGS += -I./util/container
FLAGS += -I./util
FLAGS += -I./notes
FLAGS += -I./src/ctrl

FLAGS += -fdiagnostics-color=always

# Add .cpp and .c files to the build
# This says "all cpp files are in the src folder. You can add more files
# to that folder and they will get compiled and linked also.
SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard util/quant/*.cpp)
SOURCES += $(wildcard notes/*.cpp)
SOURCES += $(wildcard util/*.cpp)

# Macro to use on any target where we don't normally want asserts
ASSERTOFF = -D NDEBUG

# make _ASSERT=true will nullify our ASSERTOFF flag, thus allowing them
ifdef _ASSERT
	ASSERTOFF =
endif

# This turns asserts off for make (plugin), not for test or perf
# except it's not working now, so will do regular
## $(TARGET) :  FLAGS += $(ASSERTOFF)
FLAGS += $(ASSERTOFF)

## $(TARGET) : FLAGS += -D __PLUGIN
FLAGS += -D __PLUGIN

# mac does not like this argument
ifdef ARCH_WIN
	FLAGS += -fmax-errors=5
endif

# Add files to the ZIP package when running `make dist`
# The compiled plugin is automatically added.
DISTRIBUTABLES += $(wildcard LICENSE*) res

# Include the VCV Rack plugin Makefile framework
# This makefile from VCV has many compiler flags and command 
# line variables. You should/must use this file.
include $(RACK_DIR)/plugin.mk

# ---- for ASAN on linux, just uncomment
# using the static library is a hack for getting the libraries to load.
# Trying other hacks first is a good idea. Building Rack.exe with asan is a pretty easy way
# ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer -Wno-format-security
# FLAGS += $(ASAN_FLAGS)
# LDFLAGS += $(ASAN_FLAGS)
# CFLAGS +=  $(ASAN_FLAGS)
# LDFLAGS += -static-libasan
