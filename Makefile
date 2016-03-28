CC=clang
CFLAGS=-c -Wall
LDFLAGS=
TARGET=dtimgextract
SOURCES=dtimgextract.c \
	qcdt_v1.c \
	qcdt_v2.c \
	qcdt_v3.c \
	qcdt_v3_coolpad.c
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
