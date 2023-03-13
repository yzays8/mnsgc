CC = g++
TARGET = main
OBJS = main.o vm.o

CXXFLAGS = -Wall

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o

.PHONY: run
run:
	./$(TARGET)

$(TARGET): $(OBJS) Makefile
	$(CC) $(OBJS) $(LIBS) -o $@

%.o: %.cpp Makefile
	$(CC) $(CXXFLAGS) -c $<