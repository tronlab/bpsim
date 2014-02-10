# Author: Tom Manville
# tdmanv@umich.edu
# 2012
#
# Please see top level directory for license.

TOP = ../..

CXX = g++
CXXFLAGS = -Wall -Werror -O3 -I$(TOP) 
CXXFLAGS = -Wall -Werror -g -I$(TOP) 

# File names
LIB = libcommon.a
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)


# Main target
$(LIB): $(COMMON_LIB) $(OBJECTS)
	ar rcs $@ $(OBJECTS) 

# To obtain object files
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY : clean
clean:
	rm -f $(LIB) $(OBJECTS)

