#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>

// Forward declarations
extern "C" {
    #include "ast.h"
}
#include "VirtualMachine.h"

typedef int ProgramID;

enum class ProgramState {
    SUBMITTED,
    PARSED,
    COMPILED,
    RUNNING,
    PAUSED,
    TERMINATED,
    ERROR
};

// IR Generator - Lab 3
class IRGenerator {
private:
    std::vector<int32_t> bytecode;
    std::map<std::string, int> variables;
    int nextVarIndex;
    int labelCounter;
    std::map<std::string, int> labelAddresses;
    std::vector<std::pair<int, std::string>> labelFixups;
    
    int allocateVar(const std::string& name);
    int getVar(const std::string& name);
    void emit(int32_t instruction);
    void emitPush(int32_t value);
    void emitLoad(const std::string& varName);
    void emitStore(const std::string& varName);
    int newLabel();
    void placeLabel(int label);
    void emitJump(int32_t opcode, int label);
    void generateExpr(ASTNode* node);
    void generateStmt(ASTNode* node);
    
public:
    IRGenerator();
    std::vector<int32_t> generate(ASTNode* root);
};

// Program representation
class Program {
public:
    ProgramID pid;
    std::string sourceFile;
    std::string sourceCode;
    ProgramState state;
    ASTNode* ast;

    std::vector<int32_t> bytecode;
    
    std::unique_ptr<VM> vm;
    
    std::string errorMessage;
    std::string output;
    
    Program(ProgramID id, const std::string& file);
    ~Program();
    
    bool loadAndParse();
    bool compile();
    bool execute();
};

class ProgramManager {
private:
    std::map<ProgramID, std::unique_ptr<Program>> programs;
    ProgramID nextPid;
    
public:
    ProgramManager();
   
    ProgramID submitProgram(const std::string& filename);
    bool runProgram(ProgramID pid);
    bool killProgram(ProgramID pid);

    bool debugProgram(ProgramID pid, const std::string& command, 
                     const std::vector<std::string>& args);
    

    void memstat(ProgramID pid);
    void gc(ProgramID pid);
    void leaks(ProgramID pid);

    void listPrograms() const;
    std::string getProgramState(ProgramID pid) const;
    std::string getProgramOutput(ProgramID pid) const;
    
private:
    Program* getProgram(ProgramID pid);
};

#endif
