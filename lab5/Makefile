# Compiler and Flags
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Target Executables
TARGET_GC      = test_gc
TARGET_EDGE    = test_edge
TARGET_VM      = vm
TARGET_ASM     = assemble

# Source Files
SRC_VM         = VirtualMachine.cpp
SRC_GC_TEST    = test_gc.cpp
SRC_EDGE_TEST  = test_gc_edge.cpp
SRC_VM_MAIN    = vm_main.cpp
SRC_ASM        = Assembler.cpp assemble.cpp

# Headers (Dependencies)
HEADERS_VM     = VirtualMachine.h Value.h Object.h Instruction.h
HEADERS_ASM    = Assembler.h Instruction.h

# Default Rule: Build Everything
all: $(TARGET_GC) $(TARGET_EDGE) $(TARGET_VM) $(TARGET_ASM)

# 1. Build the Main GC Test Suite
$(TARGET_GC): $(SRC_GC_TEST) $(SRC_VM) $(HEADERS_VM)
	$(CXX) $(CXXFLAGS) -o $(TARGET_GC) $(SRC_GC_TEST) $(SRC_VM)

# 2. Build the Advanced Edge Case Tests
$(TARGET_EDGE): $(SRC_EDGE_TEST) $(SRC_VM) $(HEADERS_VM)
	$(CXX) $(CXXFLAGS) -o $(TARGET_EDGE) $(SRC_EDGE_TEST) $(SRC_VM)

# 3. Build the Standard VM (Lab 4 Compatible + GC Enabled)
$(TARGET_VM): $(SRC_VM_MAIN) $(SRC_VM) $(HEADERS_VM)
	$(CXX) $(CXXFLAGS) -o $(TARGET_VM) $(SRC_VM_MAIN) $(SRC_VM)

# 4. Build the Assembler
$(TARGET_ASM): $(SRC_ASM) $(HEADERS_ASM)
	$(CXX) $(CXXFLAGS) -o $(TARGET_ASM) $(SRC_ASM)

# Run All Tests
test: $(TARGET_GC) $(TARGET_EDGE)
	@echo "=== Running Standard GC Tests ==="
	./$(TARGET_GC)
	@echo "\n=== Running Advanced Edge Cases ==="
	./$(TARGET_EDGE)

# Clean up build artifacts
clean:
	rm -f $(TARGET_GC) $(TARGET_EDGE) $(TARGET_VM) $(TARGET_ASM)
	rm -f *.o

.PHONY: all clean test