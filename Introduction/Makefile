TARGET=knowbot
LIBS=
CC=gcc
CFLAGS=-g -Wall -fsigned-char -fPIC -D__LINUX__ -I.
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

default: $(TARGET)

# Link
$(TARGET) : $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

# Compile
%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	-rm -f *.o
	-rm -f $(TARGET)
