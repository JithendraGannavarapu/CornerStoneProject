#include <iostream>
#include <fstream>
#include "VirtualMachine.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./vm program.byc\n";
        return 1;
    }

    ifstream in(argv[1], ios::binary);
    if (!in) {
        cerr << "Cannot open bytecode file\n";
        return 1;
    }

    in.seekg(0, ios::end);
    size_t size = in.tellg();
    in.seekg(0, ios::beg);

    vector<int32_t> bytecode(size / sizeof(int32_t));
    in.read(reinterpret_cast<char*>(bytecode.data()), size);
    in.close();

    VM vm(bytecode);
    vm.run();
    return 0;
}
