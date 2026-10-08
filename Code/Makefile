#####################################################################################
# Specify:
# (1) The folder containing the source files + name of the compiled program
# (2) Name of the program
#####################################################################################
SRC := src
BIN := program

OBJ := obj
MKDIR := mkdir -p
RM := rm -rf
SRCs := $(shell find $(SRC) -name "*.cpp")
# src/a/b.cpp -> obj/a/b.o  (patsubst: only the leading "src/" and trailing ".cpp" are replaced)
OBJs := $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCs))
DEPs := $(OBJs:.o=.d)

CXX := g++ -std=c++17
CPPFLAGS := -Iinclude -Iinclude/ann -Iinclude/tensor -Iinclude/sformat -Idemo -Isrc
# -MMD -MP: emit a .d file next to every .o so that editing ANY header
# (this project is mostly header-only) triggers the rebuild of the affected files.
DEPFLAGS := -MMD -MP
# Optional optimisation level, e.g.:  make OPT=-O2
OPT ?=
CFLAGS := -pthread $(OPT) #-Wall
LDLIBS := -lm -lpthread 
#############################################################################################
# Note: 
# (1) Use -Iinclude/tensor: because put xtensor and its headers inside of folder tensor
# (2) Use -Iinclude/sformat: because put sformat and its headers inside of folder sformat
# (3) Use -Iinclude/ann: because put header files of ann inside of folder ann
# (4) Use -Idemo: because put header files of demos inside of this folder
#############################################################################################

all: $(BIN)

$(BIN): $(OBJs)
	$(CXX) $(CFLAGS) $(OBJs) -o $@ $(LDLIBS)

# Each object file depends only on its own source file (plus the headers listed in its .d file)
$(OBJ)/%.o: $(SRC)/%.cpp
	$(MKDIR) $(dir $@)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPs)

# Clean rule to remove generated files
clean:
	$(RM) $(BIN)
	$(RM) $(OBJ)


.PHONY: all clean run
run: $(BIN)
	@echo "Running program..."
	./$(BIN)
