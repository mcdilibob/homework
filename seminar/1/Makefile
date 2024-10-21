CXX=g++
CXXFLAGS=-O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format
SOURCES=$(wildcard *.cpp)
HEADERS=autofile.h
OBJS=$(SOURCES:.cpp=.o)
EXES=$(SOURCES:%.cpp=a%.out)

all: $(EXES)

%.o: %.cpp HEADERS
	$(CXX) $(CXXFLAGS) -c $<

$(EXES) : $(OBJS)
	$(CXX) -o $@ $<

.PHONY: clean

clean:
	rm -f $(EXES) $(OBJS)
