# Lab 3: Compiler Construction

**Authors:**
* Manmandha Rao Kuttum (2025MCS2101)
* Jithendra Gannavarapu (2025MCS2743)

## 1. Overview
This project implements a front-end compiler for a custom C-like imperative programming language. It utilizes **Flex** for lexical analysis and **Bison** for parsing. The compiler validates source code syntax and semantics (such as variable declarations) and constructs a hierarchical **Abstract Syntax Tree (AST)**.

**Key Features:**
* **Types:** Integers only.
* **Control Flow:** `if`, `if-else`, `while`.
* **Scoping:** Nested block scopes `{ ... }`.
* **Output:** Hierarchical AST structure printed to stdout.

## 2. Prerequisites
Ensure you have the following installed (Linux/WSL environment):
* `gcc`
* `make`
* `flex`
* `bison`

## 3. Build Instructions
A `Makefile` is provided to automate the build process.

```bash
make        # Compiles and generates the 'parser' executable
make clean  # Removes executable and build artifacts (object files, lex.yy.c)
```
## 4. Usage

You can run the parser in three different modes:

A. Run with Input File (Recommended) Redirect a source file into the parser:

```bash
./parser < tests/valid/test01_basic.txt
```
B. Interactive Mode Type code manually into the terminal. Press Ctrl+D (EOF) to execute.

```bash
./parser
var x = 10;
var y = x * 2;
// Press Ctrl+D here to see the AST
```

C. Single Line Mode Useful for quick syntax checks:

```bash
echo "var x = 10 + 5;" | ./parser
```

5. AST Structure and Construction

The Abstract Syntax Tree (AST) represents the logic of the program hierarchically.

Node Structure: Defined in ast.h. Uses a generic ASTNode struct containing a type (e.g., NODE_BINOP, NODE_IF, NODE_VAR_DECL), values (int/string), and pointers to children.

Bottom-Up Construction: Nodes are created in the semantic actions of the Bison grammar rules.

Example: When the rule term + factor is matched, a helper function create_binop("+", $1, $3) is called to create a parent node with the two operands as children.

6. Error Handling Strategy

We implemented a Panic Mode strategy to ensure strict validation:

Lexical Errors: Unknown characters trigger an immediate error message to stderr and an exit(1).

Syntax Errors: The yyerror() function reports the line number and also triggers exit(1).

Rationale: This strict exit behavior ensures that the compiler stops immediately on the first error, preventing the generation of corrupted ASTs and ensuring the automated test suite correctly identifies invalid programs.

7. Limitations and Extensions

Limitations:

Data Types: Strictly supports integers. No floats, strings, or booleans.

Symbol Table: Linear array with a fixed limit of 1000 variables.

Scope: No function definitions; code executes in a global/main scope.

Extensions:

Code Generation: The AST is designed to be traversed for generating LLVM IR or x86 Assembly in future labs.

Type Checking: The symbol table logic can be extended to enforce type safety if new types are added.

8. Implementation & Design Decisions

Dangling Else: Resolved using Bison's default Shift behavior. The ELSE token is shifted onto the stack, binding it to the nearest open IF.

Recursive Grammar: The grammar follows a recursive structure (statement -> block -> statement_list) to allow infinite nesting depth (e.g., loops inside loops), verified by stress tests.

Symbol Table: Implemented to validate variable declarations (var x) and prevent using undeclared variables.

9. Testing

The project includes an automated test script running 20 test cases (10 valid, 10 invalid).

```bash
chmod +x test_suites.sh
./test_suites.sh
```
[PASS] Valid Tests: Returns Exit Code 0.

[PASS] Invalid Tests: Returns Exit Code 1 (correctly rejected).

10. Directory Structure

.
├── Makefile            # Build automation
├── README.md           # Documentation
├── test_suites.sh      # Test automation script
├── src/
│   ├── lexer.l         # Flex lexical rules
│   ├── parser.y        # Bison grammar & symbol table
│   ├── ast.h           # AST Node definitions
│   └── ast.c           # AST helper functions
└── tests/
    ├── valid/          # Test cases that should pass
    └── invalid/        # Test cases that should fail