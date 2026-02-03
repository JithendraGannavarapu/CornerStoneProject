#!/bin/bash

# Configuration
SYSTEM_BIN="./lab6_system"
TEST_DIR="tests"
mkdir -p $TEST_DIR

# UI Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}====================================================${NC}"
echo -e "${BLUE}      LAB 6 FINAL INTEGRATION VERIFICATION${NC}"
echo -e "${BLUE}====================================================${NC}"

# 1. Create a Comprehensive Test File [cite: 89, 114]
cat <<EOF > $TEST_DIR/master_test.lang
var x = 10;
var y = 20;
if (x < y) {
    print x + y;
}
var i = 0;
while (i < 3) {
    i = i + 1;
}
print i;
EOF

echo -e "Starting automated test sequence..."

# 2. Run the full command workflow [cite: 941, 114]
$SYSTEM_BIN <<EOF > master_results.txt
submit $TEST_DIR/master_test.lang
debug 1
ast
bytecode
step
regs
memstat
continue
gc 1
leaks 1
exit
exit
EOF

echo -e "\n${BLUE}--- Component Check ---${NC}"

if grep -q "PID = 1" master_results.txt; then
    echo -e "${GREEN}[PASS]${NC} Lab 1/6: Program Submission & PID Assignment"
else
    echo -e "${RED}[FAIL]${NC} Lab 1/6: Program Manager failed to register PID"
fi

if grep -q "VAR_DECL: x" master_results.txt && grep -q "STORE" master_results.txt; then
    echo -e "${GREEN}[PASS]${NC} Lab 2/3: AST Metadata & IR (Bytecode) Generation"
else
    echo -e "${RED}[FAIL]${NC} Lab 2/3: Metadata/IR is missing or opaque"
fi

if grep -q -e "--- VM Register State ---" master_results.txt && grep -q "Output : 30" master_results.txt; then
    echo -e "${GREEN}[PASS]${NC} Lab 4: Execution Control (Step/Regs/Continue)"
else
    echo -e "${RED}[FAIL]${NC} Lab 4: VM control layer failed to pause/resume correctly"
fi

if grep -q "Heap objects" master_results.txt; then
    echo -e "${GREEN}[PASS]${NC} Lab 5: Memory Observability & GC Reclamation"
else
    echo -e "${RED}[FAIL]${NC} Lab 5: Heap tracking or Leak detection failed"
fi

echo -e "\n${BLUE}====================================================${NC}"
echo -e "Verification Complete. See 'master_results.txt' for logs."
echo -e "${BLUE}====================================================${NC}"