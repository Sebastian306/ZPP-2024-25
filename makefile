# Compiler flags
CPPFLAGS = -Wall -g -Wextra -std=c++20 -O2
COMPILER = g++

# Subdirectory for TCtester component
TT_FOLDER = terrain-type-detection

# Object files for TCtester
TC_OBJECTS = common.o $(TT_FOLDER)/terrainChecker.o $(TT_FOLDER)/TCtester.o

# TCtester executable
TCtester: $(TC_OBJECTS)
	$(COMPILER) $(CPPFLAGS) -o $@ $^

# Compilation rules for .cpp files
common.o: common.cpp
	$(COMPILER) $(CPPFLAGS) -c $< -o $@

$(TT_FOLDER)/terrainChecker.o: $(TT_FOLDER)/terrainChecker.cpp
	$(COMPILER) $(CPPFLAGS) -c $< -o $@

$(TT_FOLDER)/TCtester.o: $(TT_FOLDER)/TCtester.cpp
	$(COMPILER) $(CPPFLAGS) -c $< -o $@

# Clean rule
.PHONY: clean
clean:
	rm -f $(TC_OBJECTS) TCtester
