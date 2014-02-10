# Author: Tom Manville
# tdmanv@umich.edu
# 2012
#
# Please see top level directory for license.

PRED = example

TOP = .

PRED_DIR = $(TOP)/predictors/$(PRED)
PRED_LIB = $(PRED_DIR)/libpred.a

CXX = g++ 
CXXFLAGS = -I$(PRED_DIR) -Wall -Werror -O3 
CXXFLAGS = -I$(PRED_DIR) -Wall -Werror -g

COMMON_DIR = $(TOP)/common
COMMON_LIB = $(COMMON_DIR)/libcommon.a

BOOST_LIBS = /usr/lib/libboost_iostreams.a /usr/lib/x86_64-linux-gnu/libz.a /usr/lib/libboost_program_options.a 

LDFLAGS = -L$(PRED_DIR) -L$(COMMON_DIR) -lpred -lcommon $(BOOST_LIBS)

# File names
EXEC = cbp3
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY : main
main: 
	make -C $(COMMON_DIR)
	make -C $(PRED_DIR)
	make $(EXEC)_$(PRED)


$(EXEC)_$(PRED): $(EXEC)
	mv $(EXEC) $(EXEC)_$(PRED).exe

$(EXEC): $(OBJECTS) $(PRED_DIR)/libpred.a $(COMMON_LIB)
	$(CXX) $(OBJECTS) $(COMMON_LIB) $(LDFLAGS) -o $(EXEC) 


# To obtain object files
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@



.PHONY : clean
clean: 
	rm -f $(EXEC) $(OBJECTS) *.exe
	make clean -C $(PRED_DIR)
	make clean -C $(COMMON_DIR)


PREDS = pc_sweep
.PHONY : all
all:
	$(foreach var,$(PREDS),make PRED=$(var);)
.PHONY : clean_all
clean_all:
	$(foreach var,$(PREDS),make clean PRED=$(var);)

