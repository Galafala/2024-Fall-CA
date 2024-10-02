# Define the compiler and the target executable
CC = gcc
TARGET = main

# The default rule compiles and runs the program
all: $(TARGET)
	./$(TARGET) 19

# Rule to compile the main.c file into the main executable
$(TARGET): main.c
	$(CC) -o $(TARGET) main.c

# Clean up the generated files
clean:
	rm -f $(TARGET)