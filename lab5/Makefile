# Compiler and Flags
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Targets
TARGETS  = assemble vm

# Default rule: Build everything
all: $(TARGETS)

# Rule to build the Assembler
assemble: Assembler.cpp assemble.cpp Assembler.h Instruction.h
	$(CXX) $(CXXFLAGS) -o assemble Assembler.cpp assemble.cpp

# Rule to build the Virtual Machine
vm: VirtualMachine.cpp vm_main.cpp VirtualMachine.h Instruction.h
	$(CXX) $(CXXFLAGS) -o vm VirtualMachine.cpp vm_main.cpp

# Rule to run tests (Assuming you have the run.sh script)
test: all
	chmod +x run.sh
	./run.sh

# Clean up build artifacts and bytecode files
clean:
	rm -f $(TARGETS)
	rm -f tests/*.byc

.PHONY: all clean test