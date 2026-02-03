#include "program_manager.h"
#include "Instruction.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

extern "C" {
    extern FILE* yyin;
    extern int yyparse();
    extern int parsing_failed;
    extern ASTNode* root;
    extern void clear_parser_symbols();
}

IRGenerator::IRGenerator() : nextVarIndex(0), labelCounter(0) {}

int IRGenerator::allocateVar(const std::string& name) {
    if (variables.find(name) == variables.end()) {
        variables[name] = nextVarIndex++;
    }
    return variables[name];
}

int IRGenerator::getVar(const std::string& name) {
    auto it = variables.find(name);
    return (it == variables.end()) ? -1 : it->second;
}

void IRGenerator::emit(int32_t instruction) {
    bytecode.push_back(instruction);
}

void IRGenerator::emitPush(int32_t value) {
    emit(OP_PUSH);
    emit(value);
}

void IRGenerator::emitLoad(const std::string& varName) {
    int idx = getVar(varName);
    if (idx == -1) idx = allocateVar(varName);
    emit(OP_LOAD);
    emit(idx);
}

void IRGenerator::emitStore(const std::string& varName) {
    int idx = allocateVar(varName);
    emit(OP_STORE);
    emit(idx);
}

int IRGenerator::newLabel() {
    return labelCounter++;
}

void IRGenerator::placeLabel(int label) {
    std::string labelName = "L" + std::to_string(label);
    labelAddresses[labelName] = (int32_t)bytecode.size();
    for (auto& fixup : labelFixups) {
        if (fixup.second == labelName) {
            bytecode[fixup.first] = (int32_t)bytecode.size();
        }
    }
}

void IRGenerator::emitJump(int32_t opcode, int label) {
    std::string labelName = "L" + std::to_string(label);
    emit(opcode);
    auto it = labelAddresses.find(labelName);
    if (it != labelAddresses.end()) {
        emit(it->second);
    } else {
        labelFixups.push_back({(int)bytecode.size(), labelName});
        emit(0);
    }
}

void IRGenerator::generateExpr(ASTNode* node) {
    if (!node) {
        emitPush(0);
        return;
    }
    switch (node->type) {
        case NODE_INT:
            emitPush(node->value);
            break;
        case NODE_ID:
            emitLoad(node->name);
            break;
        case NODE_BINOP: {
            generateExpr(node->left);
            generateExpr(node->right);
            if (strcmp(node->op, "+") == 0) emit(OP_ADD);
            else if (strcmp(node->op, "-") == 0) emit(OP_SUB);
            else if (strcmp(node->op, "*") == 0) emit(OP_MUL);
            else if (strcmp(node->op, "/") == 0) emit(OP_DIV);
            else if (strcmp(node->op, "<") == 0) emit(OP_CMP);
            else if (strcmp(node->op, "==") == 0) {
                emit(OP_SUB);
                emit(OP_JZ);
            }
            break;
        }
        case NODE_UNARY: {
            generateExpr(node->left);
            if (node->op && strcmp(node->op, "-") == 0) {
                emitPush(-1);
                emit(OP_MUL);
            }
            break;
        }
        default: break;
    }
}

void IRGenerator::generateStmt(ASTNode* node) {
    if (!node) return;
    switch (node->type) {
        case NODE_STMT_LIST:
            if (node->left) generateStmt(node->left);
            if (node->right) generateStmt(node->right);
            break;
        case NODE_VAR_DECL:
            if (node->left) generateExpr(node->left);
            else emitPush(0);
            emitStore(node->name);
            break;
        case NODE_ASSIGN:
            generateExpr(node->left);
            emitStore(node->name);
            break;
        case NODE_UNARY:
            if (node->op && strcmp(node->op, "print") == 0) {
                generateExpr(node->left);
                emit(OP_PRINT);
            }
            break;
        case NODE_IF: {
            int elseLabel = newLabel();
            int endLabel = newLabel();
            generateExpr(node->condition);
            emitJump(OP_JZ, elseLabel);
            generateStmt(node->body);
            if (node->else_body) {
                emitJump(OP_JMP, endLabel);
                placeLabel(elseLabel);
                generateStmt(node->else_body);
                placeLabel(endLabel);
            } else {
                placeLabel(elseLabel);
            }
            break;
        }
        case NODE_WHILE: {
            int startLabel = newLabel();
            int endLabel = newLabel();
            placeLabel(startLabel);
            generateExpr(node->condition);
            emitJump(OP_JZ, endLabel);
            generateStmt(node->body);
            emitJump(OP_JMP, startLabel);
            placeLabel(endLabel);
            break;
        }
        default: break;
    }
    if (node->next) generateStmt(node->next);
}

std::vector<int32_t> IRGenerator::generate(ASTNode* root) {
    bytecode.clear();
    variables.clear();
    nextVarIndex = 0;
    labelCounter = 0;
    labelAddresses.clear();
    labelFixups.clear();
    generateStmt(root);
    emit(OP_HALT);
    return bytecode;
}

Program::Program(ProgramID id, const std::string& file)
    : pid(id), sourceFile(file), state(ProgramState::SUBMITTED), ast(nullptr) {}

Program::~Program() {
    if (ast) free_ast(ast);
}

bool Program::loadAndParse() {
    clear_parser_symbols();
    std::ifstream file(sourceFile);
    if (!file) {
        errorMessage = "Cannot open file: " + sourceFile;
        state = ProgramState::ERROR;
        return false;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    sourceCode = ss.str();
    file.close();
    FILE* temp = fopen("/tmp/parse_input.txt", "w");
    if (temp) {
        fprintf(temp, "%s", sourceCode.c_str());
        fclose(temp);
    }
    yyin = fopen("/tmp/parse_input.txt", "r");
    if (!yyin) return false;
    root = nullptr;
    parsing_failed = 0;
    if (yyparse() != 0 || parsing_failed) {
        errorMessage = "Parse failed";
        state = ProgramState::ERROR;
        fclose(yyin);
        return false;
    }
    fclose(yyin);
    ast = root;
    state = (ast) ? ProgramState::PARSED : ProgramState::ERROR;
    return ast != nullptr;
}

bool Program::compile() {
    if (state != ProgramState::PARSED) return false;
    IRGenerator irGen;
    bytecode = irGen.generate(ast);
    state = ProgramState::COMPILED;
    return true;
}

bool Program::execute() {
    if (state != ProgramState::COMPILED) return false;
    vm = std::make_unique<VM>(bytecode);
    state = ProgramState::RUNNING;
    vm->run();
    state = ProgramState::TERMINATED;
    output = "Program completed execution.\n";
    return true;
}

ProgramManager::ProgramManager() : nextPid(1) {}

ProgramID ProgramManager::submitProgram(const std::string& filename) {
    ProgramID pid = nextPid++;
    auto prog = std::make_unique<Program>(pid, filename);
    if (!prog->loadAndParse() || !prog->compile()) {
        programs[pid] = std::move(prog);
        return pid;
    }
    programs[pid] = std::move(prog);
    return pid;
}

bool ProgramManager::runProgram(ProgramID pid) {
    Program* prog = getProgram(pid);
    if (!prog || prog->state != ProgramState::COMPILED) return false;
    bool success = prog->execute();
    if (success && prog->vm) {
        prog->vm->printStats();
    }
    return success;
}

bool ProgramManager::debugProgram(ProgramID pid, const std::string& command, const std::vector<std::string>& args) {
    (void)args;
    Program* prog = getProgram(pid);
    if (!prog) return false;
    if (!prog->vm) prog->vm = std::make_unique<VM>(prog->bytecode);

    if (command == "step") {
        prog->vm->executeNext();
    }else if( command == "print") {
        prog->vm->printFinalStack();
    } else if (command == "regs") {
        prog->vm->printRegisters();
    }else if (command == "break") {
        if (args.size() < 1) {
            std::cout << "Usage: break <address>\n";
            return false;
        }
        int addr = std::stoi(args[0]);
        prog->vm->setBreakpoint(addr);
    } else if (command == "clear") {
        if (args.size() < 1) {
            std::cout << "Usage: clear <address>\n";
            return false;
        }
        int addr = std::stoi(args[0]);
        prog->vm->clearBreakpoint(addr);
    }else if (command == "run") {
        while (prog->vm->isRunning()) {
            prog->vm->executeNext();
        }
    }else if (command == "halt") {
        prog->vm->stop();
    }else if (command == "next") {
        size_t initialPC = prog->vm->getPC();
        prog->vm->executeNext();
        while (prog->vm->isRunning() && prog->vm->getPC() == initialPC) {
            prog->vm->executeNext();
        }
    }else if (command == "info") {
        prog->vm->printStats();
    }
    else if (command == "continue") {
        prog->vm->run();
    } else if (command == "memstat") {
        prog->vm->printHeapStatus();
    } else if (command == "state") {
        std::cout << "State: " << getProgramState(pid) << "\n";
    } else if (command == "bytecode") {
        for (size_t i = 0; i < prog->bytecode.size(); ++i) {
            int32_t op = prog->bytecode[i];
            std::string name = VM::getOpcodeName(op);
            printf("%3zu: 0x%02x (%s)", i, op, name.c_str());
            if (name == "PUSH " || name == "JMP  " || name == "JZ   " || name == "LOAD " || name == "STORE") {
                if (i + 1 < prog->bytecode.size()) printf(" %d", prog->bytecode[++i]);
            }
            printf("\n");
        }
    } else if (command == "ast") {
        if (prog->ast) print_ast(prog->ast, 0);
    }
    return true;
}

void ProgramManager::memstat(ProgramID pid) {
    Program* prog = getProgram(pid);
    if (prog && prog->vm) prog->vm->printHeapStatus();
}

void ProgramManager::gc(ProgramID pid) {
    Program* prog = getProgram(pid);
    if (prog && prog->vm) prog->vm->gc();
}

void ProgramManager::leaks(ProgramID pid) {
    Program* prog = getProgram(pid);
    if (prog && prog->vm) {
        std::cout << "Active Objects: " << prog->vm->getObjectCount() << "\n";
    }
}

void ProgramManager::listPrograms() const {
    for (const auto& [pid, prog] : programs) {
        std::cout << "PID " << pid << " [" << getProgramState(pid) << "]: " << prog->sourceFile << "\n";
    }
}
bool ProgramManager::killProgram(ProgramID pid) {
    Program* prog = getProgram(pid);
    if (!prog) return false;

    if (prog->vm) {
        prog->vm->stop();
    }
    
    prog->state = ProgramState::TERMINATED;
    std::cout << "PID " << pid << " terminated by user.\n";
    return true;
}
std::string ProgramManager::getProgramState(ProgramID pid) const {
    auto it = programs.find(pid);
    if (it == programs.end()) return "NONE";
    switch (it->second->state) {
        case ProgramState::SUBMITTED: return "SUBMITTED";
        case ProgramState::PARSED: return "PARSED";
        case ProgramState::COMPILED: return "COMPILED";
        case ProgramState::RUNNING: return "RUNNING";
        case ProgramState::TERMINATED: return "TERMINATED";
        case ProgramState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string ProgramManager::getProgramOutput(ProgramID pid) const {
    auto it = programs.find(pid);
    return (it != programs.end()) ? it->second->output : "";
}

Program* ProgramManager::getProgram(ProgramID pid) {
    auto it = programs.find(pid);
    return (it != programs.end()) ? it->second.get() : nullptr;
}