CC = gcc
CFLAGS = -Wall -g
OBJ = observer.o weather_station.o
TARGET = weather_station

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

observer.o: observer.c observer.h
	$(CC) $(CFLAGS) -c observer.c

weather_station.o: weather_station.c observer.h
	$(CC) $(CFLAGS) -c weather_station.c

clean:
	rm -f $(OBJ) $(TARGET)