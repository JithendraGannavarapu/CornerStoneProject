#include "Assembler.h"
#include "Instruction.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cctype>

using namespace std;

static bool isNumber(const string& s) {
    if (s.empty()) return false;
    size_t i = (s[0] == '-' ? 1 : 0);
    for (; i < s.size(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

static void stripComments(string& line) {
    size_t pos = line.find_first_of("#;");
    if (pos != string::npos)
        line = line.substr(0, pos);
}

static int instructionSize(const string& instr) {
    if (instr == "PUSH" || instr == "JMP" || instr == "JZ" ||
        instr == "JNZ"  || instr == "CALL" ||
        instr == "LOAD" || instr == "STORE")
        return 2;
    return 1;
}

vector<int32_t> assemble(const string& filename) {

    unordered_map<string, int32_t> opcodeMap = {
        {"PUSH", OP_PUSH}, {"POP", OP_POP}, {"DUP", OP_DUP},
        {"ADD", OP_ADD},   {"SUB", OP_SUB}, {"MUL", OP_MUL},
        {"DIV", OP_DIV},   {"CMP", OP_CMP},
        {"JMP", OP_JMP},   {"JZ", OP_JZ},   {"JNZ", OP_JNZ},
        {"CALL", OP_CALL}, {"RET", OP_RET},
        {"LOAD", OP_LOAD}, {"STORE", OP_STORE}, 
        {"HALT", OP_HALT}
    };

    ifstream file(filename);
    if (!file) {
        cerr << "Cannot open assembly file\n";
        exit(1);
    }

    vector<string> lines;
    string line;

    while (getline(file, line)) {
        stripComments(line);
        if (!line.empty())
            lines.push_back(line);
    }
    file.close();

    unordered_map<string, int> labelTable;
    int pc = 0;

    for (const string& l : lines) {
        stringstream ss(l);
        string token;
        ss >> token;
        if (token.empty()) continue;
        if (token.back() == ':') {
            string label = token.substr(0, token.size() - 1);
            if (labelTable.count(label)) {
                cerr << "Duplicate label: " << label << endl;
                exit(1);
            }
            labelTable[label] = pc;
            continue;
        }

        if (!opcodeMap.count(token)) {
            cerr << "Invalid instruction: " << token << endl;
            exit(1);
        }

        pc += instructionSize(token);
    }

    vector<int32_t> bytecode;

    for (const string& l : lines) {
        stringstream ss(l);
        string instr;
        ss >> instr;
        if (instr.empty() || instr.back() == ':')
            continue;

        int32_t opcode = opcodeMap[instr];
        bytecode.push_back(opcode);

        if (instructionSize(instr) == 2) {
            string operand;
            ss >> operand;

            if (operand.empty()) {
                cerr << "Missing operand for " << instr << endl;
                exit(1);
            }

            if (isNumber(operand)) {
                bytecode.push_back(stoi(operand));
            } else {
                if (!labelTable.count(operand)) {
                    cerr << "Undefined label: " << operand << endl;
                    exit(1);
                }
                bytecode.push_back(labelTable[operand]);
            }
        }
    }

    return bytecode;
}
