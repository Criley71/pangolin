#pragma once
#include "parser.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <vector>
#include <cstring>
using namespace std;

class Executor {
public:
    int execute(const ASTNode* node);

private:
    int execute_command(const ASTNode* node);
    int execute_pipe(const ASTNode* node);
};