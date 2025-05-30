CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -g -O0

TARGET = main

SRCS = main.cpp cpu.cpp instructions.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
