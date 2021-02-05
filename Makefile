TARGET = PMan
OBJFILES = PMan.o LinkedList.o helpers.o
CFLAGS = -Wall
LDFLAGS = -lreadline -lhistory

.phony all:

all: $(TARGET)

$(TARGET): $(OBJFILES)
	gcc $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

.PHONY clean:
clean:
	-rm -rf *.o *.exe