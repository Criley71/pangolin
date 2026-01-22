#pragma once
#include "parser.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <vector>
#include <cstring>
#include "commands.h"
using namespace std;

class Executor {
public:
    int execute(ASTNode* node);
    void expand_commands(vector<string>& argv);
private:
    int execute_command(ASTNode* node);
    int execute_pipe(ASTNode* node);
};