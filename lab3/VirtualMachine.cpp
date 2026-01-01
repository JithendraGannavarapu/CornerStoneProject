#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

enum Opcode {
    OP_PUSH = 0x01,
    OP_POP  = 0x02,
    OP_DUP  = 0x03,
    OP_HALT = 0xFF
};

class VM {
private:
    vector<int32_t> program;  
    vector<int32_t> stack;     
    int pc;                   
    bool running;

public:
    VM(const vector<int32_t>& bytecode)
        : program(bytecode), pc(0), running(true) {}

    void run() {
        while (running) {
            int32_t opcode = fetch();
            execute(opcode);
        }
    }

    void printStack() {
        cout << "Stack: [ ";
        for (int32_t v : stack) {
            cout << v << " ";
        }
        cout << "]\n";
    }


private:
    int32_t fetch() {
        return program[pc++];
    }

    void execute(int32_t opcode) {
        switch (opcode) {

            case OP_PUSH: {
                int32_t value = fetch();
                stack.push_back(value);
                cout << "Executed PUSH " << value << "\n";
                printStack();
                break;
            }

            case OP_POP: {
                if (stack.empty()) {
                    cerr << "POP on empty stack\n";
                    running = false;
                    break;
                }
                stack.pop_back();
                cout << "Executed POP\n";
                printStack();
                break;
            }

            case OP_DUP: {
                if (stack.empty()) {
                    cerr << "DUP on empty stack\n";
                    running = false;
                    break;
                }
                stack.push_back(stack.back());
                cout << "Executed DUP\n";
                printStack();
                break;
            }

            case OP_HALT:
                cout << "Executed HALT\n";
                running = false;
                break;

            default:
                cerr << "Unknown opcode\n";
                running = false;
        }
    }

};

int main() {
    vector<int32_t> bytecode = {
        OP_PUSH, 10,
        OP_DUP,
        OP_POP,
        OP_HALT
    };

    VM vm(bytecode);
    vm.run();

    cout << "Step 2 executed successfully." << endl;
    return 0;
}
