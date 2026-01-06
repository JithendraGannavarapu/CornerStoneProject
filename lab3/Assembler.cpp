#include "Assembler.h"
#include "Instruction.h"
#include <fstream>
#include <unordered_map>
#include <iostream>

using namespace std;

vector<int32_t> assemble(const string& filename) {
    unordered_map<string, int32_t> opcodeMap = {
        {"PUSH", OP_PUSH},
        {"POP", OP_POP},
        {"DUP", OP_DUP},
        {"ADD", OP_ADD},
        {"SUB", OP_SUB},
        {"MUL", OP_MUL},
        {"DIV", OP_DIV},
        {"CMP", OP_CMP},
        {"JMP", OP_JMP},
        {"JZ",  OP_JZ},
        {"JNZ", OP_JNZ},
        {"HALT", OP_HALT}

    };

    vector<int32_t> bytecode;
    ifstream file(filename);

    if (!file) {
        cerr << "Cannot open assembly file\n";
        exit(1);
    }

    string instr;
    while (file >> instr) {
        if (opcodeMap.count(instr) == 0) {
            cerr << "Invalid instruction: " << instr << endl;
            exit(1);
        }

        bytecode.push_back(opcodeMap[instr]);
        if (instr == "PUSH" || instr == "JMP" ||
        instr == "JZ"   || instr == "JNZ") {

        int32_t val;
        file >> val;
        bytecode.push_back(val);
    }

    }

    return bytecode;
}
