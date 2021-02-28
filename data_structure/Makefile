CC = gcc
CFLAGS = -c -Wall
LFLAGS = --coverage
SOURCES = structure.c test.c
OBJECTS = $(SOURCES:.cpp=.o)
LOG = log.txt
INFO = test.info
REPORT = report
TRASH = *.d *.o *.h.gch
EXECUTABLE = run
.PHONY = clean

all: build link test clean

build: $(SOURCES)

link: $(EXECUTABLE)

test:
	./$(EXECUTABLE) > $(LOG)
	lcov -t $(EXECUTABLE) -o $(INFO) -c -d
	genhtml -o $(REPORT) $(INFO)
	valgrind ./$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $(OBJECTS)

$(SOURCES):
	$(CC) $(CFLAGS) -o $@ -c $< -MD

clean:
		rm -rf $(TRASH) $(LOG) $(REPORT)

-include *.d
