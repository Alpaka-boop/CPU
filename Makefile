CC = g++

TARGET = proc

ALL = main.cpp proc.cpp stack.cpp

all: $(ALL)
	$(CC) $(ALL) $(FLAGS) -Wall -Wextra -o $(TARGET)
