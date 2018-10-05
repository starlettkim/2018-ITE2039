# Compiler and Compile options.
CC = g++ 
CXXFLAGS = -g -Wall -std=c++11

# Macros specifying path for compile.
SRCS := project1-2.cpp
OBJS := $(SRCS:.cpp=.o)

# Compile command.
TARGET = project1-2
$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Delete binary & object files.
clean:
	rm $(TARGET) $(OBJS)