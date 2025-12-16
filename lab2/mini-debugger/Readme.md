=============================================================================
LAB 2A: MINIMAL DEBUGGER | CORNERSTONE PROJECT (ASSIGNMENT 2)
=============================================================================

TEAM MEMBERS
-----------------------------------------------------------------------------
1. Manmandha Rao Kuttum     (Roll No: 2025MCS2101)
2. Jithendra Gannavarapu    (Roll No: 2025MCS2743)

SUBMISSION DATE: December 16, 2025

=============================================================================
1. PROJECT OVERVIEW
=============================================================================
This project is a minimal ELF debugger implemented in C for the x86_64 architecture.
It leverages the Linux kernel's `ptrace` API to provide execution control and 
state inspection of target binaries. The debugger runs without reliance on 
external tools like GDB.

=============================================================================
2. DIRECTORY STRUCTURE & FILES INCLUDED
=============================================================================
The submission folder (Lab2) contains the following:

[SOURCE CODE]
 - debugger.c             : Main source code for the debugger logic.
 - sample_code.c          : A simple target program used for testing.
 - Makefile               : Script to compile both the debugger and target.

[DOCUMENTATION]
 - Technical_Report.pdf   : Detailed report on ptrace usage, breakpoint logic
                            (INT 3 injection), and parent-child synchronization.

[EVIDENCE]
 - Demo Screenshots/      : Folder containing proof of execution.
   ├── Screenshot 1.png   : Loading binary and address lookup.
   ├── Screenshot 2.png   : Breakpoint hit and single-stepping.
   ├── Git-Graph.png      : Version control history.

=============================================================================
3. COMPILATION AND EXECUTION
=============================================================================
System Requirements: Linux Environment (Ubuntu/WSL) with GCC installed.

Step 1: Open a terminal inside this directory.
Step 2: Compile the project using Make.
        $ make
        (This compiles 'dbg' and the 'target' binary with PIE disabled).

Step 3: Run the debugger with the target binary.
        $ ./dbg ./target

Step 4: (Optional) Clean build files.
        $ make clean

=============================================================================
4. SUPPORTED COMMANDS
=============================================================================
Once inside the debugger prompt (dbg>), you can use:

 - break <hex_addr> : Set a breakpoint (e.g., break 0x401136).
 - run / continue   : Resume execution until next breakpoint or exit.
 - step             : Execute a single machine instruction (Single Step).
 - regs             : Display current Instruction Pointer (RIP).
 - info bp          : Show current breakpoint details.
 - clear            : Remove the active breakpoint.
 - quit             : Detach and exit.

=============================================================================
5. TECHNICAL HIGHLIGHTS
=============================================================================
A. Breakpoint Injection:
   - Uses `PTRACE_PEEKDATA` to backup the instruction.
   - Uses `PTRACE_POKEDATA` to inject the 0xCC (INT 3) trap opcode.

B. Execution Control:
   - `PTRACE_CONT`: Standard resume.
   - `PTRACE_SINGLESTEP`: Hardware-assisted single instruction execution.

C. State Management ("Fix RIP" Logic):
   - When a breakpoint is hit, the CPU stops *after* the trap.
   - The debugger automatically rewinds the Instruction Pointer (RIP = RIP - 1)
     and restores the original instruction so execution can proceed smoothly.

D. Parent-Child Sync:
   - Uses `waitpid` macros (WIFSTOPPED, WSTOPSIG) to strictly differentiate
     between Breakpoint hits (SIGTRAP) and program crashes.

=============================================================================
6. GITHUB REPOSITORY
=============================================================================
The full development history and source code are available at:
https://github.com/JithendraGannavarapu/CornerStone-Project/tree/main/lab2/mini-debugger

=============================================================================