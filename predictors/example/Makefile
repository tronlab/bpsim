# Author: Tom Manville
# tdmanv@umich.edu
# 2012
#
# Please see top level directory for license.

TOP = ../..
COMMON_DIR = $(TOP)/common

CXX = g++
CXXFLAGS = -Wall -Werror -O3 -I$(TOP) -I$(COMMON_DIR)
CXXFLAGS = -Wall -Werror -g -I$(TOP) -I$(COMMON_DIR)

# File names
LIB = libpred.a
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)


# Main target
$(LIB): $(OBJECTS)
	ar rcs $@ $^

# To obtain object files
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@


.PHONY : clean
clean:
	rm -f $(LIB) $(OBJECTS)

