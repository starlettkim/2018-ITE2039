# Compiler and Compile options.
CC = g++ 
CXXFLAGS = -O3 -Wall -std=c++14

# Macros specifying path for compile.
SRCS := project4.cpp
OBJS := $(SRCS:.cpp=.o)

# Compile command.
TARGET = project4
$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lpthread

# Delete binary & object files.
clean:
	rm $(TARGET) $(OBJS)