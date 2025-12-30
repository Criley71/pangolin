#include "../include/repl.h"

void REPL::repl_loop() {
  string user_in = "";
  string buf = "";
  vector<string> input = {};
  shell_startup();
  while (true) {
    input = {};
    repl_dir_print();

    getline(cin, user_in);
    stringstream ss(user_in);
    while (ss >> buf) {
      input.push_back(buf);
    }

    // cout << "you entered: ";
    if (input[0] == "exit") {
      exit(EXIT_SUCCESS);
    }
    const char *command = input[0].c_str();
    vector<char *> args = {};
    for (uint32_t i = 0; i < input.size(); i++) {
      args.push_back(const_cast<char *>(input[i].c_str()));
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
    cout << "\n";
    ss.clear();
  }
}
// █
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
    bool match_found = false;
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
  string temp;
  getline(cin, temp);
}

void REPL::repl_dir_print() {
  char buffer[2048];
  if (getcwd(buffer, sizeof(buffer)) != NULL) {
    cout << "\033[34m(pangolin)\033[32m " << buffer + 1 << "\033[34m$\033[0m ";
  }
}

bool REPL::is_outline(const string &s) {
  static const vector<string> outlines = {u8"╔", u8"╗", u8"╚", u8"╝", u8"║", u8"═"};
  for (const auto &o : outlines) {
    if (s == o) return true;
  }
  return false;
}


//TODO: MAKE OWN SET OF FILES FOR START UP SCREEN TO REMOVE FROM REPL PART
//ADD IN ALIAS COMMAND
//ADD IN CD AND EXIT