#include "../include/repl.h"




void REPL::repl_loop() {

  string user_in = "";
  string buf = "";
  vector<string> input = {};
  shell_startup();
  Commands Command;
  char buffer[2048];
  string prompt;
  while (true) {
    getcwd(buffer, sizeof(buffer)); 
    input = {};
    //repl_dir_print();
    prompt = "\033[34m(pangolin)\033[32m" + string(buffer) + "\033[34m$\033[0m ";
    char* line = readline(prompt.c_str());
    //getline(cin, user_in);
    stringstream ss(line);
    while (ss >> buf) {
      input.push_back(buf);
    }

    // cout << "you entered: ";
    if (input[0] == "exit") {
      exit(EXIT_SUCCESS);
    }

    if(is_built_in(input[0])){
      Command.determine_command(input);
      add_history(line);
      continue;
    }
    const char *command = input[0].c_str();
    vector<char *> args = {};
    for (uint32_t i = 0; i < input.size(); i++) {
      args.push_back(const_cast<char *>(input[i].c_str()));
    }
    if(is_aliased(input[0])){
      for(auto c : aliases[input[0]]){
        args.push_back(const_cast<char *>(c.c_str()));
      }
    }
    args.push_back(nullptr);
    pid_t child_pid = fork();
    if (child_pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (child_pid > 0) {
      wait(NULL);
      // parent process
    } else {
      // child process
      execvp(command, args.data());
      perror("execvp");
      _exit(EXIT_FAILURE);
    }
    add_history(line);
    cout << "\n";
    ss.clear();
  }
  
}

// 158, 72, 68
// 66, 63, 79
void REPL::shell_startup() {
  // cout << " \033[48;2;158;72;68m " << logo << "     \033[0m\n";
  string RED = "\033[38;2;158;72;68m";
  string BLUE = "\033[38;2;66;63;79m";
  string RESET = "\033[0m";
  string block = "█";
  vector<string> blues = {"╗", "║", "╝", "═", "╚", "╔", "░"};
  for (size_t i = 0; i < logo.size(); i++) {
    bool matchFound = false;

    if (logo.substr(i, block.length()) == block) {
      cout << RED << block << RESET;
      i += (block.length() - 1);
      matchFound = true;
    }

    if (!matchFound) {
      for (const string &shape : blues) {
        if (logo.substr(i, shape.length()) == shape) {
          cout << BLUE << shape << RESET;
          i += (shape.length() - 1);
          matchFound = true;
          break;
        }
      }
    }
    if (!matchFound) {
      cout << logo[i];
    }
  }
  cout << "\033[38;2;158;72;68m⠀⣀⠀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀                   \n"
          "⣿⡀⢻⣿⣿⣿⣿⣿⣶⠀⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
          "⠛⠓⠀⢻⣿⡿⠋⣉⣀⣀⢘⣿⣿⣿⡇⢠⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
          "⣾⣿⣿⣿⣿⠀⣾⣿⣿⣿⣿⡿⠟⠛⠧⠈⢿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
          "⣿⣿⣿⡿⠿⠄⠹⣿⣿⣿⡃⢰⣶⣶⣶⣦⣴⣿⣿⠇⣀⠀⠀⠀⠀⠀⠀⠀⠀\n"
          "⣿⡏⢠⣴⣶⣦⣤⣼⣿⣿⡷⠄⠉⠉⣉⡉⠛⢿⡏⢰⣿⣦⡀⠀⠀⠀\033[37m⠀⠀⠀\033[4m                               \033[24m\033[38;2;158;72;68m\n"
          "⣿⡀⢿⣿⣿⣿⣿⣿⣿⠋⣠⣶⣧⡈⠛⠻⣷⣄⠁⠘⣿⣿⣷⡀⠀⠀\033[37m⠀⠀/⠀                              \\\n\033[38;2;158;72;68m"
          "⢉⠁⠘⢿⣿⣿⣿⣿⡇⢰⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠈⠛⢿⣷⡀⠀\033[37m⠀\uff5c Welcome to the Pangolin Shell \uff5c\033[38;2;158;72;68m\n"
          "⣿⣿⣷⣿⡿⠛⣉⡉⠁⢸⠃⠈⠙⠻⢿⣿⣿⣿⣷⣶⣾⣷⣄⠙⠳⠀⠀\033[37m⠀\\\033[4m                               \033[24m/\033[38;2;158;72;68m\n"
          "⣿⣿⣿⣿⡀⢾⣿⣿⡇⠘⠀⠀⠀⠀⠀⠈⠉⠛⠻⠿⣿⣿⣿⣿⣦⡀⠀⠀\033[37m⠀|/\033[38;2;158;72;68m\n"
          "⠿⠋⣉⣉⡁⠈⠻⣿⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠀⠉⠀⠀⠀⠀\n"
          "⠀⣾⣿⣿⣿⣷⣶⣿⡏⢡⣤⣤⠀⢀⣤⣄⡐⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
          "⠀⠙⢿⣿⣿⡿⠛⣉⣁⣀⣿⠃⠐⠛⠿⣿⣷⠀⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
          "⠀⠀⠀⠀⠈⠉⠃⠘⠿⠟⠛⠋⠀⠀⠀⠀⢹⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
          "\033[37m";
  // string temp;
  // getline(cin, temp);
}

void REPL::repl_dir_print() {
  char buffer[2048];
  if (getcwd(buffer, sizeof(buffer)) != NULL) {
    cout << "\033[34m(pangolin)\033[32m " << buffer+1 << "\033[34m$\033[0m ";
  }
}



bool REPL::is_built_in(string command) {
  for (size_t i = 0; i < builtin_commands.size(); i++) {
    if (builtin_commands[i] == command){
      return true;
    };
  }
  return false;
}

bool REPL::is_aliased(string command){
  for(auto c : aliases){
    if(c.first == command){
      return true;
    }
  }
  return false;
}

// TODO:
// ADD IN CLOCK STUFF FOR EXIT AND START UP, MAYBE MAKE AN DIGITAL CLOCK, print time of command like current shell on far right 
// ADD IN CD AND EXIT
// ADD IN ALIAS COMMAND