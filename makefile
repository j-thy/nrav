CC = g++
CFLAGS = -std=c++11 -Wall
DEPS = nrav.h
TARGET = nrav
OBJ = main.o nrav.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) *.o $(TARGET)