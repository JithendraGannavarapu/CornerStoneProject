#include <iostream>
#include "Assembler.h"
#include "VirtualMachine.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: ./vm program.asm\n";
        return 1;
    }

    vector<int32_t> bytecode = assemble(argv[1]);
    VM vm(bytecode);
    vm.run();

    return 0;
}
