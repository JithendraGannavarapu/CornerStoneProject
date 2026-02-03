#!/bin/bash

# 1. Create Directory Structure
mkdir -p tests/valid
mkdir -p tests/invalid

echo "Generating 10 Valid Test Cases..."

# --- VALID TEST CASES ---

# 1. Basic Declaration & Assignment
cat <<EOF > tests/valid/test01_basic.txt
var x = 10;
var y = 20;
x = y;
EOF

# 2. Arithmetic Precedence (Multiplication before Addition)
cat <<EOF > tests/valid/test02_math.txt
var res = 0;
res = 2 + 3 * 4; 
// Expected: 14 (not 20)
EOF

# 3. Simple If-Else
cat <<EOF > tests/valid/test03_ifelse.txt
var x = 10;
if (x < 20) {
    x = x + 1;
} else {
    x = 0;
}
EOF

# 4. While Loop
cat <<EOF > tests/valid/test04_loop.txt
var i = 0;
while (i < 5) {
    i = i + 1;
}
EOF

# 5. Nested Control Flow
cat <<EOF > tests/valid/test05_nested.txt
var x = 0;
var y = 10;
while (x < 5) {
    if (y > 5) {
        y = y - 1;
    }
    x = x + 1;
}
EOF

# 6. Block Scoping
cat <<EOF > tests/valid/test06_scope.txt
var x = 10;
{
    var z = 5;
    x = x + z;
}
EOF

# 7. Complex Comparison
cat <<EOF > tests/valid/test07_compare.txt
var x = 10;
var y = 20;
var z = 30;
if (x < y) {
    if (y < z) {
        x = z;
    }
}
EOF

# 8. Division and Subtraction
cat <<EOF > tests/valid/test08_div_sub.txt
var x = 100;
x = x - 50 / 2;
// Expected: 75
EOF

# 9. Variable Initialization with Expression
cat <<EOF > tests/valid/test09_init_expr.txt
var x = 10;
var y = x * 2 + 5;
EOF

# 10. Empty Block (Edge Case)
cat <<EOF > tests/valid/test10_empty.txt
var x = 10;
while (x < 0) {
    // Do nothing
}
EOF


echo "Generating 10 Invalid Test Cases..."

# --- INVALID TEST CASES ---

# 1. Use Before Declaration
cat <<EOF > tests/invalid/fail01_undeclared.txt
x = 10;
EOF

# 2. Duplicate Declaration
cat <<EOF > tests/invalid/fail02_double_decl.txt
var x = 10;
var x = 20;
EOF

# 3. Missing Semicolon
cat <<EOF > tests/invalid/fail03_no_semi.txt
var x = 10
EOF

# 4. Invalid Syntax (Bad Assignment Target)
cat <<EOF > tests/invalid/fail04_bad_assign.txt
var x = 10;
10 = x;
EOF

# 5. Unmatched Parenthesis
cat <<EOF > tests/invalid/fail05_paren.txt
if (x > 5 {
    x = 0;
}
EOF

# 6. Unmatched Brace
cat <<EOF > tests/invalid/fail06_brace.txt
while (x < 10) {
    x = x + 1;
EOF

# 7. Invalid Character
cat <<EOF > tests/invalid/fail07_bad_char.txt
var x = 100;
x = x % 2; 
// % is not in our grammar
EOF

# 8. Using Keyword as Identifier
cat <<EOF > tests/invalid/fail08_keyword.txt
var if = 10;
EOF

# 9. Missing Operand
cat <<EOF > tests/invalid/fail09_missing_op.txt
var x = 10 + ;
EOF

# 10. Malformed If
cat <<EOF > tests/invalid/fail10_bad_if.txt
if x < 10 {
    x = 0;
}
// Missing parentheses around condition
EOF

echo "Files created successfully in tests/ directory."

# --- RUNNER ---
echo "--- Running Tests ---"
make > /dev/null

echo "Checking Valid Tests..."
for f in tests/valid/*.txt; do
    ./parser < "$f" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "[PASS] $(basename $f)"
    else
        echo "[FAIL] $(basename $f)"
    fi
done

echo "Checking Invalid Tests (Should all fail)..."
for f in tests/invalid/*.txt; do
    ./parser < "$f" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "[PASS] $(basename $f) (Correctly rejected)"
    else
        echo "[FAIL] $(basename $f) (Should have failed but passed)"
    fi
done