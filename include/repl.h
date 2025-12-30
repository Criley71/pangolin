#ifndef REPL_H
#define REPL_H
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <string_view>
using namespace std;

struct REPL {
  void repl_loop();
  void shell_startup();
  void repl_dir_print();
  bool is_outline(const string &s);
   

  string logo = R"(
██████╗░░█████╗░███╗░░░██╗░██████╗░░██████╗░██╗░░░░░██╗███╗░░░██╗
██╔═██║░██╔══██╗████╗░░██║██╔════╝░██╔═══██╗██║░░░░░██║████╗░░██║
██████║░███████║██╔██╗░██║██║░░███╗██║░░░██║██║░░░░░██║██╔██╗░██║
██╔═══╝░██╔══██║██║╚██╗██║██║░░░██║██║░░░██║██║░░░░░██║██║╚██╗██║
██║░░░░░██║░░██║██║░╚████║╚██████╔╝╚██████╔╝███████╗██║██║░╚████║
╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═══╝░╚═════╝░░╚═════╝░╚══════╝╚═╝╚═╝░░╚═══╝

)";
};

#endif