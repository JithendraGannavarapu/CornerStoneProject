CXX = g++
CC = gcc
# Added -I02_Parser so program_manager.cpp can find parser.tab.h
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I. -I01_Shell -I02_Parser -I03_Compiler -I04_VM_Execution -I05_Memory_GC
CFLAGS = -Wall -Wextra -g -I. -I01_Shell -I02_Parser -I03_Compiler -I04_VM_Execution -I05_Memory_GC

TARGET = lab6_system
TEST_GC = test_gc
TEST_GC_EDGE = test_gc_edge

# VPATH allows Make to find source files in these subdirectories
VPATH = 01_Shell:02_Parser:03_Compiler:04_VM_Execution:05_Memory_GC

# Source files (removed parser_wrapper.c to fix duplicate symbols)
LAB6_SRCS_CPP = lab6_main.cpp program_manager.cpp VirtualMachine.cpp
LAB6_SRCS_C = ast.c parser.tab.c lex.yy.c

# Object files
OBJS = $(LAB6_SRCS_CPP:.cpp=.o) $(LAB6_SRCS_C:.c=.o)

all: $(TARGET) $(TEST_GC) $(TEST_GC_EDGE)

# Link the main integrated system
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Build complete: ./$(TARGET)"


02_Parser/parser.tab.c 02_Parser/parser.tab.h: 02_Parser/parser.y
	bison -d 02_Parser/parser.y -o 02_Parser/parser.tab.c

02_Parser/lex.yy.c: 02_Parser/lexer.l 02_Parser/parser.tab.h
	flex -o 02_Parser/lex.yy.c 02_Parser/lexer.l


parser.tab.o: 02_Parser/parser.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

lex.yy.o: 02_Parser/lex.yy.c
	$(CC) $(CFLAGS) -c $< -o $@

lab6_main.o: lab6_main.cpp program_manager.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

program_manager.o: program_manager.cpp program_manager.h ast.h VirtualMachine.h Instruction.h 02_Parser/parser.tab.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

VirtualMachine.o: VirtualMachine.cpp VirtualMachine.h Value.h Object.h Instruction.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_GC): test_gc.cpp VirtualMachine.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_GC_EDGE): test_gc_edge.cpp VirtualMachine.o
	$(CXX) $(CXXFLAGS) -o $@ $^

test_files:
	@mkdir -p tests
	@echo "var a = 10; var b = 20; var c = a + b; print c;" > tests/basic.lang
	@echo "var i = 0; while(i < 3) { i = i + 1; print i; }" > tests/loop.lang
	@echo "var x = 100; if(x > 50) { x = 1; } else { x = 0; } print x;" > tests/logic.lang
	@echo "Syntax Error Here" > tests/error.lang

test: $(TARGET) test_files
	@echo "===================================================="
	@echo "RUNNING FULL LAB 6 INTEGRATION SUITE"
	@echo "===================================================="
	@printf "help\n\
	submit tests/basic.lang\n\
	submit tests/loop.lang\n\
	list\n\
	run 1\n\
	debug 2\n\
	step\n\
	memstat\n\
	continue\n\
	exit\n\
	submit tests/error.lang\n\
	list\n\
	exit\n" > /tmp/lab6_suite.txt
	-@./$(TARGET) < /tmp/lab6_suite.txt
	@echo "===================================================="
	@echo "TEST SUITE COMPLETE"
	@echo "===================================================="

clean:
	rm -f $(TARGET) $(TEST_GC) $(TEST_GC_EDGE) *.o
	rm -f 02_Parser/parser.tab.c 02_Parser/parser.tab.h 02_Parser/lex.yy.c
	rm -f tests/*.lang /tmp/lab6_suite.txt /tmp/parse_input.txt
	@echo "✓ Cleaned artifacts and generated parser files"

.PHONY: all clean test test_files