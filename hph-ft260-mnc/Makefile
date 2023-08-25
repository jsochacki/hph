TARGET_EXEC ?= hph-ft260-mnc.exe

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
#INC_DIRS += ./lib
INC_DIRS += /usr/local/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

LDFLAGS+=-lhidapi-libusb
#YOU MUST USE libusb OR ELSE YOU GET NO FUNCTIONALITY AND NO DUAL INTERFACE PRESENTATION
#LDFLAGS+=-lhidapi-hidraw
# Need this for string library and wchar_t operations
LDFLAGS+=-lstdc++
# Need this for math library operations
LDFLAGS+=-lm

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

LIBRARIES_PATH:=/usr/local/lib
LIBRARIES_FLAG:=$(addprefix -L,$(LIBRARIES_PATH))

RPATH_PATH:=/usr/local/lib
RPATH_FLAGS:=-Wl,$(addprefix -rpath=,$(RPATH_PATH))

ifeq ($(origin CC),default)
CC  = gcc
endif

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBRARIES_FLAG) $(RPATH_FLAGS) $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

print:
	echo CC=$(CC) CFLAGS=$(CFLAGS)
	echo INC_DIRS=$(INC_DIRS)
	echo RPATH_FLAGS=$(RPATH_FLAGS)

-include $(DEPS)

MKDIR_P ?= mkdir -p
