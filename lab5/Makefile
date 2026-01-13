# Compiler and Flags
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Target Executables
TARGET_GC      = test_gc
TARGET_VM      = vm
TARGET_ASM     = assemble

# Source Files
SRC_VM         = VirtualMachine.cpp
SRC_GC_TEST    = test_gc.cpp
SRC_VM_MAIN    = vm_main.cpp
SRC_ASM        = Assembler.cpp assemble.cpp

# Headers (Dependencies)
HEADERS_VM     = VirtualMachine.h Value.h Object.h Instruction.h
HEADERS_ASM    = Assembler.h Instruction.h

# Default Rule: Build Everything
all: $(TARGET_GC) $(TARGET_VM) $(TARGET_ASM)

# 1. Build the GC Test Driver (Lab 5 Specific)
$(TARGET_GC): $(SRC_GC_TEST) $(SRC_VM) $(HEADERS_VM)
	$(CXX) $(CXXFLAGS) -o $(TARGET_GC) $(SRC_GC_TEST) $(SRC_VM)

# 2. Build the Standard VM (Compatible with Lab 4 bytecode)
$(TARGET_VM): $(SRC_VM_MAIN) $(SRC_VM) $(HEADERS_VM)
	$(CXX) $(CXXFLAGS) -o $(TARGET_VM) $(SRC_VM_MAIN) $(SRC_VM)

# 3. Build the Assembler (Standard Lab 4 Tool)
$(TARGET_ASM): $(SRC_ASM) $(HEADERS_ASM)
	$(CXX) $(CXXFLAGS) -o $(TARGET_ASM) $(SRC_ASM)

# Run the GC Test Suite
test: $(TARGET_GC)
	./$(TARGET_GC)

# Clean up build artifacts
clean:
	rm -f $(TARGET_GC) $(TARGET_VM) $(TARGET_ASM)
	rm -f *.o

.PHONY: all clean test