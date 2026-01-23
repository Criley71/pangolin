#include "../include/executor.h"

void Executor::expand_commands(vector<string> &argv) {
  for (auto &arg : argv) {
    if (!arg.empty() && arg[0] == '$') {
      const char *val = getenv(arg.c_str() + 1);
      arg = val ? val : "";
    }
  }

  if (!argv.empty() && argv[0] == "ls") {
    bool has_color_flag = false;
    for (const auto &arg : argv) {
      if (arg.find("--color") != string::npos) {
        has_color_flag = true;
        break;
      }
    }
    if (!has_color_flag) {
      argv.insert(argv.begin() + 1, "--color=auto");
    }
  }
  const char *home = getenv("HOME");
  //if (!home) return;
  for (auto &arg : argv) {
    if (!arg.empty() && arg[0] == '~') {
      if (arg.size() == 1 || arg[1] == '/') {
        arg = string(home) + arg.substr(1);
      }
    }
  }
}

int Executor::execute_command(ASTNode *node) {
  if (node->argv.empty()) {
    return 0;
  }
  Commands command;
  vector<char *> argv;
  expand_commands(node->argv);
  for (const auto &arg : node->argv) {
    argv.push_back(const_cast<char *>(arg.c_str()));
  }
  argv.push_back(nullptr);
  if (command.determine_command(node->argv)) {
    return 1;
  }

  pid_t pid = fork();
  if (pid == 0) {
    execvp(argv[0], argv.data());
    cerr << "EXEC: [" << argv[0] << "]\n";
    perror("execvp");
    _exit(127);
  }

  int status;
  waitpid(pid, &status, 0);

  if (WIFEXITED(status))
    return WEXITSTATUS(status);

  return 1;
}

int Executor::execute_pipe(ASTNode *node) {
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

int Executor::execute(ASTNode *node) {
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
    case NodeType::OR_IF: {
      int status = execute(node->left.get());
    if (status != 0)
      return execute(node->right.get());
    return status;
    }
  }

  throw runtime_error("Unknown AST node type");
}
