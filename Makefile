CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0

TARGET = main

SRCS = main.cpp cpu.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
