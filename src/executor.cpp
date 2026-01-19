#include "../include/executor.h"




int Executor::execute_command(const ASTNode* node) {
  if (node->argv.empty()) {
    return 0;
  }
  
  vector<char*> argv;
  for (const auto& arg : node->argv) {
    argv.push_back(const_cast<char*>(arg.c_str()));
  }
  argv.push_back(nullptr);
  
  pid_t pid = fork();
  if (pid == 0) {
    execvp(argv[0], argv.data());
    perror("execvp");
    _exit(127);
  }
  
  int status;
  waitpid(pid, &status, 0);
  
  if (WIFEXITED(status))
  return WEXITSTATUS(status);
  
  return 1;
}

int Executor::execute_pipe(const ASTNode* node) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe");
    return 1;
  }
  
  pid_t left = fork();
  if (left == 0) {
    dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    close(fd[1]);
    _exit(execute(node->left.get()));
  }
  
  pid_t right = fork();
  if (right == 0) {
    dup2(fd[0], STDIN_FILENO);
    close(fd[1]);
    close(fd[0]);
    _exit(execute(node->right.get()));
  }
  
  close(fd[0]);
  close(fd[1]);
  
  int status;
  waitpid(left, &status, 0);
  waitpid(right, &status, 0);
  
  if (WIFEXITED(status))
  return WEXITSTATUS(status);
  
  return 1;
}

int Executor::execute(const ASTNode* node) {
    if (!node) return 0;

    switch (node->type) {
    case NodeType::COMMAND:
        return execute_command(node);

    case NodeType::PIPE:
        return execute_pipe(node);

    case NodeType::AND_IF: {
        int status = execute(node->left.get());
        if (status == 0)
            return execute(node->right.get());
        return status;
    }
    }

    throw runtime_error("Unknown AST node type");
}