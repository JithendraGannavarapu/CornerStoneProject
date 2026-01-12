#include <iostream>
#include "Assembler.h"
#include <vector>
#include <cstdint>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./assemble program.asm\n";
        return 1;
    }

    string asmFile = argv[1];
    auto bytecode = assemble(asmFile);

    string outFile = asmFile.substr(0, asmFile.find_last_of('.')) + ".byc";

    ofstream out(outFile, ios::binary);
    out.write(reinterpret_cast<const char*>(bytecode.data()),
              bytecode.size() * sizeof(int32_t));
    out.close();

    cout << "Assembled " << asmFile << " → " << outFile << endl;
    return 0;
}
