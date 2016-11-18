CPP = g++

TARGET_EXEC ?= app.exe

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

ROOT_CPPFLAGS := $(shell root-config --cflags)
ROOT_INC_FLAGS := $(shell root-config --glibs)

SRCS := $(shell find $(SRC_DIRS) | grep .cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) $(ROOT_INC_FLAGS)

CPPFLAGS ?= $(INC_FLAGS) -g -Wall $(ROOT_CPPFLAGS)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CPP) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CPP) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
