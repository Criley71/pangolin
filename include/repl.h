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
#include <signal.h>
#include <fstream>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stack>
#include "lexer.h"
#include "parser.h"
#include "executor.h"
#include <queue>
using namespace std;

struct REPL {
  void repl_loop();
  void shell_startup();
  void repl_dir_print();
  bool is_built_in(string command);
  bool is_aliased(string command);
  void check_dup_add_history(char* command);
  void init_readline();
  void setup_signals();
  bool unknown_command_contains_slash(const string& command);
  bool found_in_path(const string& command);
  string tilde_translation(string arg);
  void load_history();
  string get_history_dir();
  void repl2();
  
  //void handle_sigint(int);
  //void init_signals();
  //volatile sig_atomic_t sigint_recieved = 0;
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

      /*
⣿⠿⣿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⠀⢿⡄⠀⠀⠀⠀⠀⠉⣿⠛⠻⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣤⣬⣿⡄⠀⢀⣴⠶⠿⠿⡧⠀⠀⠀⢸⡟⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⠁⠀⠀⠀⠀⣿⠁⠀⠀⠀⠀⢀⣠⣤⣘⣷⡀⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⠀⠀⠀⢀⣀⣻⣆⠀⠀⠀⢼⡏⠉⠉⠉⠙⠋⠀⠀⣸⠿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⠀⢰⡟⠋⠉⠙⠛⠃⠀⠀⢈⣻⣶⣶⠶⢶⣤⡀⢰⡏⠀⠙⢿⣿⣿⣿⣿⣿⣿
⣿⠀⢿⡀⠀⠀⠀⠀⠀⠀⣴⠟⠉⠘⢷⣤⣄⠈⠻⣾⣧⠀⠀⠈⢿⣿⣿⣿⣿⣿
⣿⡶⣾⣧⡀⠀⠀⠀⠀⢸⡏⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⣷⣤⡀⠈⢿⣿⣿⣿⣿
⣿⠀⠀⠈⠀⢀⣤⠶⢶⣾⡇⣼⣷⣦⣄⡀⠀⠀⠀⠈⠉⠁⠈⠻⣦⣌⣿⣿⣿⣿
⣿⠀⠀⠀⠀⢿⡁⠀⠀⢸⣧⣿⣿⣿⣿⣿⣷⣶⣤⣄⣀⠀⠀⠀⠀⠙⢿⣿⣿⣿
⣿⣀⣴⠶⠶⢾⣷⣄⠀⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣿⣿⣿⣿⣿
⣿⣿⠁⠀⠀⠀⠈⠉⠀⢰⡞⠛⠛⣿⡿⠛⠻⢯⡙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣦⡀⠀⠀⢀⣤⠶⠾⠿⠀⣼⣯⣤⣀⠀⠈⣿⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣷⣶⣼⣧⣀⣠⣤⣴⣿⣿⣿⣿⡆⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
      */
private:
  vector<string> builtin_commands{"cd", "exit", "help", "pwd"};
  unordered_map<string, vector<string>> aliases = {
      {"ls", {"--color=auto"}},
      {"grep", {"--color=auto"}}};
};
#endif