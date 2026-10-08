# This source is currently a Hello World placeholder; it does not use the KAYA SDK.
CXX ?= g++
CXXFLAGS ?= -O2 -g

.PHONY: all clean
all: case_5288

case_5288: case_5288.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -std=c++11 "$<" -o "$@" $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) case_5288
