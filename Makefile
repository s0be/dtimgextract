CC=clang
CFLAGS=-c -Wall
LDFLAGS=
TARGET=dtimgextract
SOURCES=dtimgextract.c
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
