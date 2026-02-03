# Define the source directory
SRC = src

# The final executable name
TARGET = parser

# Default target
all: $(TARGET)

# Link everything together
# We explicitly tell GCC to find files in the 'src' folder
$(TARGET): $(SRC)/lex.yy.c $(SRC)/parser.tab.c $(SRC)/ast.c $(SRC)/main.c
	gcc -o $(TARGET) $(SRC)/lex.yy.c $(SRC)/parser.tab.c $(SRC)/ast.c $(SRC)/main.c -lfl

# Compile Flex
# Output (-o) goes into src/lex.yy.c so it sits next to the header files
$(SRC)/lex.yy.c: $(SRC)/lexer.l $(SRC)/parser.tab.h
	flex -o $(SRC)/lex.yy.c $(SRC)/lexer.l

# Compile Bison
# Output (-o) goes into src/parser.tab.c (and creates parser.tab.h there too)
$(SRC)/parser.tab.c $(SRC)/parser.tab.h: $(SRC)/parser.y
	bison -d $(SRC)/parser.y -o $(SRC)/parser.tab.c

# Clean up generated files inside src/ and the executable in root
clean:
	rm -f $(TARGET) $(SRC)/lex.yy.c $(SRC)/parser.tab.c $(SRC)/parser.tab.h