CC = g++
CFLAGS = -c 
LFLAGS = --no-warnings -pthread -D_GNU_SOURCE
SOURCES = integral.c test.c
OBJECTS = $(SOURCES:.cpp=.o)
TRASH = *.d *.o *.h.gch *.jpg pic1 pic2 pic3
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
