#ifndef REPL_H
#define REPL_H
#include "commands.h"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <readline/history.h>
#include <readline/keymaps.h>
#include <readline/readline.h>
#include <sstream>
#include <string>
#include <string_view>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
using namespace std;

struct REPL {
  void repl_loop();
  void shell_startup();
  void repl_dir_print();
  bool is_built_in(string command);
  bool is_aliased(string command);
  string logo = R"(
    ██████╗░░█████╗░███╗░░░██╗░██████╗░░██████╗░██╗░░░░░██╗███╗░░░██╗
    ██╔═██║░██╔══██╗████╗░░██║██╔════╝░██╔═══██╗██║░░░░░██║████╗░░██║
    ██████║░███████║██╔██╗░██║██║░░███╗██║░░░██║██║░░░░░██║██╔██╗░██║
    ██╔═══╝░██╔══██║██║╚██╗██║██║░░░██║██║░░░██║██║░░░░░██║██║╚██╗██║
    ██║░░░░░██║░░██║██║░╚████║╚██████╔╝╚██████╔╝███████╗██║██║░╚████║
    ╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═══╝░╚═════╝░░╚═════╝░╚══════╝╚═╝╚═╝░░╚═══╝
    
    )";

  string icon = R"(
      \033[38;2;158;72;68m⠀⣀⠀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀                   
      ⣿⡀⢻⣿⣿⣿⣿⣿⣶⠀⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
      ⠛⠓⠀⢻⣿⡿⠋⣉⣀⣀⢘⣿⣿⣿⡇⢠⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
      ⣾⣿⣿⣿⣿⠀⣾⣿⣿⣿⣿⡿⠟⠛⠧⠈⢿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
      ⣿⣿⣿⡿⠿⠄⠹⣿⣿⣿⡃⢰⣶⣶⣶⣦⣴⣿⣿⠇⣀⠀⠀⠀⠀⠀⠀⠀⠀
      ⣿⡏⢠⣴⣶⣦⣤⣼⣿⣿⡷⠄⠉⠉⣉⡉⠛⢿⡏⢰⣿⣦⡀⠀⠀⠀\033[37m⠀⠀⠀\033[4m                               \033[24m\033[38;2;158;72;68m
      ⣿⡀⢿⣿⣿⣿⣿⣿⣿⠋⣠⣶⣧⡈⠛⠻⣷⣄⠁⠘⣿⣿⣷⡀⠀⠀\033[37m⠀⠀/⠀                              \\\033[38;2;158;72;68m
      ⢉⠁⠘⢿⣿⣿⣿⣿⡇⢰⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠈⠛⢿⣷⡀⠀\033[37m⠀\uff5c Welcome to the Pangolin Shell \uff5c\033[38;2;158;72;68m
      ⣿⣿⣷⣿⡿⠛⣉⡉⠁⢸⠃⠈⠙⠻⢿⣿⣿⣿⣷⣶⣾⣷⣄⠙⠳⠀⠀\033[37m⠀\\\033[4m                               \033[24m/\033[38;2;158;72;68m
      ⣿⣿⣿⣿⡀⢾⣿⣿⡇⠘⠀⠀⠀⠀⠀⠈⠉⠛⠻⠿⣿⣿⣿⣿⣦⡀⠀⠀\033[37m⠀|/\033[38;2;158;72;68m"
      ⠿⠋⣉⣉⡁⠈⠻⣿⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠀⠉⠀⠀⠀⠀
      ⠀⣾⣿⣿⣿⣷⣶⣿⡏⢡⣤⣤⠀⢀⣤⣄⡐⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
      ⠀⠙⢿⣿⣿⡿⠛⣉⣁⣀⣿⠃⠐⠛⠿⣿⣷⠀⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
      ⠀⠀⠀⠀⠈⠉⠃⠘⠿⠟⠛⠋⠀⠀⠀⠀⢹⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
      \033[37m
      )";

private:
  vector<string> builtin_commands{"cd", "exit", "help", "pwd"};
  unordered_map<string, vector<string>> aliases = {
      {"ls", {"--color=auto"}},
      {"grep", {"--color=auto"}}};
};
#endif