TARGET_EXEC ?= hph.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
#INC_DIRS += ./lib
INC_DIRS += /usr/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

LDFLAGS+=-lhidapi-hidraw


CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

ifeq ($(origin CC),default)
CC  = gcc
endif

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

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

-include $(DEPS)

MKDIR_P ?= mkdir -p
