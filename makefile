CC = gcc
CPP = g++
CFLAGS = -I .  -Wall -Wextra -O3 -std=c++2a -pthread -lwiringPi -lstdc++fs
OBJ =  Button.o Instruction.o JobInfo.o main.o Serial.o
DEPS = 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CFLAGS)

BoxJig: $(OBJ)
	$(CPP) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f  Button.o Instruction.o JobInfo.o main.o Serial.o
