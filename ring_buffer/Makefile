CC = gcc
CFLAGS = -c -Wall
LFLAGS = --no-warnings
SOURCES = buffer.c
OBJECTS = $(SOURCES:.cpp=.o)
TRASH = *.d *.o *.h.gch *.jpg run pic1 pic2 pic3
EXECUTABLE = run
.PHONY = clean

all: build link clean

build: $(SOURCES)

link: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $(OBJECTS)

$(SOURCES):
	$(CC) $(CFLAGS) -o $@ -c $< -MD

clean:
		rm -rf $(TRASH)

-include *.d
