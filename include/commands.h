#ifndef COMMANDS
#define COMMANDS
#include "repl.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <chrono>
using namespace std;

struct Commands{
  bool determine_command(vector<string>& command);
  void cd(string path);
  void exit_shell();
  
};

#endif