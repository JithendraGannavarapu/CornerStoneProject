# Lab 5: Mark-Sweep Garbage Collector

## Overview
This project extends the Lab 4 Stack-Based Virtual Machine with a **Stop-the-World Mark-Sweep Garbage Collector (GC)**. The VM now supports dynamic memory management, heap allocation, and automatic reclamation of unreachable objects.

## Student Details
* **Name:** Jithendra Gannavarapu
* **Roll No:** 2025MCS2743
* **Teammate:** Manmadha Rao Kuttum (2025MCS2101)

---

## Key Features (Lab 5)
* **Heap Allocator:** Dynamic allocation of `Pair` objects (O(1) insertion).
* **Mark-Sweep GC:**
    * **Mark Phase:** Recursive DFS to trace reachable objects from Roots.
    * **Sweep Phase:** Reclaims memory from unmarked (dead) objects.
* **Root Discovery:** Scans both the **VM Stack** and **Global Memory** for live references.
* **Memory Safety:** Automatically handles cyclic references and cleans up all memory on exit.

---

## Build Instructions
A `Makefile` is provided to compile all components.

1. **Clean previous builds:**
   ```bash
   make clean
Compile everything (VM, Assembler, and Tests):

Bash

make
How to Run Tests
We have provided two test suites to verify the GC implementation.

1. Run All Tests (Recommended)
This command runs both the standard validation suite and the advanced edge cases.

Bash

make test
2. Manual Execution
Standard Tests: Covers requirements 1.6.1 – 1.6.7 (Reachability, Deep Recursion, Stress).

Bash

./test_gc
Edge Case Tests: Covers Orphaned Cycles, Diamond Graphs, and Self-References.

Bash

./test_edge
How to Run the VM
The VM is fully backwards compatible with Lab 4 bytecode.

Assemble a program:

Bash

./assemble tests/program.asm
Execute the bytecode:

Bash

./vm tests/program.byc
(Note: The GC runs automatically in the background if the C++ source invokes it, or it can be triggered manually in custom test drivers.)

File Structure
VirtualMachine.h/cpp: Core VM logic, including the integrated Garbage Collector.

Value.h: Tagged union supporting both int and Object*.

Object.h: Structure for Heap Objects (ObjPair).

test_gc.cpp: Main validation suite (7 required tests).

test_gc_edge.cpp: Advanced scenarios (Cycles, Shared Children).

Assembler.cpp: Lab 4 Assembler (unchanged).