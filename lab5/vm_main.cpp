#include <iostream>
#include <fstream>
#include <vector>
#include "VirtualMachine.h"

using namespace std;

vector<int32_t> readBytecode(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }

    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    if (size % 4 != 0) {
        cerr << "Error: File size is not a multiple of 4 bytes." << endl;
        exit(1);
    }

    vector<int32_t> buffer(size / 4);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        cerr << "Error: Failed to read bytecode." << endl;
        exit(1);
    }

    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <bytecode_file>" << endl;
        return 1;
    }

    string filename = argv[1];
    vector<int32_t> bytecode = readBytecode(filename);

    VM vm(bytecode);
    vm.run();
    vm.printFinalStack();
    vm.printStats();

    return 0;
}