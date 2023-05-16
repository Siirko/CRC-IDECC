TARGET=tests
CC ?= gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lm -pthread


INCLUDE_PATH = ../include

SRCDIR   = ../src
OBJDIR   = obj

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCLUDE_PATH)/*.h)
OBJECTS0  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJECTS = $(filter-out $(OBJDIR)/main.o,$(OBJECTS0))

all: $(TARGET)

tests: $(OBJECTS) obj/tests.o

	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
	@echo "Linking complete!"


$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS) -I$(INCLUDE_PATH)

obj/tests.o: tests.c
	$(CC) -o $@ -c $< $(CFLAGS) -I$(INCLUDE_PATH)


.PHONY: clean 
clean:
	rm -f $(OBJDIR)/*.o
	rm -f ./$(TARGET)