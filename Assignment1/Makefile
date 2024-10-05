# Define the compiler and the target executable
CC = gcc
TARGET = main
SOURCE = main.c print_utils.c float_utils.c

# Define the object files (by replacing .c with .o)
OBJECTS = $(SOURCES:.c=.o)

# The default rule compiles and runs the program
all: $(TARGET)
	./$(TARGET)

# Rule to compile the source files into the main executable
$(TARGET): $(SOURCE) $(OBJECTS)
	$(CC) -o $(TARGET) $(SOURCE) $(OBJECTS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) -c $<

# Clean up the generated files
clean:
	rm -f $(TARGET)