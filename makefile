CC ?= gcc
CFLAGS ?= -Wextra -Wall -pedantic
LDLIBS ?= -lm -pthread

EXEC = project-rio

INCLUDE_PATH ?= ./include

SRC_PATH = ./src
OBJ_PATH = ./obj
BIN_PATH = ./bin

SOURCES  := $(wildcard $(SRC_PATH)/*.c)
INCLUDES := $(wildcard $(INCLUDE_PATH)/*.h)
OBJECTS  := $(SOURCES:$(SRC_PATH)/%.c=$(OBJ_PATH)/%.o)


all: $(BIN_PATH)/$(EXEC)

$(BIN_PATH)/$(EXEC): $(OBJECTS)
	mkdir -p $(BIN_PATH)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJ_PATH)/%.o : $(SRC_PATH)/%.c
	mkdir -p $(OBJ_PATH)
	$(CC) -o $@ -c $< $(CFLAGS) -I$(INCLUDE_PATH)


.PHONY: clean
clean:
	rm -fr $(OBJ_PATH)
	rm -fr $(BIN_PATH)