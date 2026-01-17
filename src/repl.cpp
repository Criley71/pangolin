#include "../include/repl.h"

volatile sig_atomic_t sigint_recieved = 0;

void handle_sigint(int) {
  sigint_recieved = 1;

  rl_replace_line("", 0);
  rl_crlf();
  rl_on_new_line();
  rl_redisplay();
}

void REPL::repl_loop() {

  string user_in = "";
  string buf = "";
  vector<string> input = {};
  shell_startup();
  Commands Command;
  char buffer[2048];
  string prompt;
  int last_status = 0;
  while (true) {
    sigint_recieved = 0;
    getcwd(buffer, sizeof(buffer));
    input = {};

    prompt = "\033[34m(pangolin)\033[32m" + string(buffer) + "\033[34m$\033[0m ";
    char *line = readline(prompt.c_str());
    if (!line) {
      break; // ctrl+d
    }
    if (sigint_recieved) {
      free(line);
      continue;
    }
    stringstream ss(line);
    while (ss >> buf) {
      input.push_back(buf);
    }

    if (input[0] == "exit") {
      exit(EXIT_SUCCESS);
    }

    if (is_built_in(input[0])) {
      Command.determine_command(input);

      check_dup_add_history(line);
      continue;
    }

    const char *command = input[0].c_str();
    vector<char *> args = {};
    for (uint32_t i = 0; i < input.size(); i++) {
      args.push_back(const_cast<char *>(input[i].c_str()));
    }

    if (is_aliased(input[0])) {
      for (auto c : aliases[input[0]]) {
        args.push_back(const_cast<char *>(c.c_str()));
      }
    }

    args.push_back(nullptr);
    if (!unknown_command_contains_slash(input[0]) && !found_in_path(input[0])) {
      cout << "pangolin: command not found: " << input[0] << "\n";
      last_status = 127;
      continue;
    }
    pid_t child_pid = fork();
    if (child_pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (child_pid > 0) {
      wait(NULL);
      // parent process
    } else {
      // child process
      signal(SIGINT, SIG_DFL); // kill child on ctrl+c
      execvp(args[0], args.data());
      switch (errno) {
      case ENOENT:
        fprintf(stderr, "pangolin: command not found: %s\n", command);
        _exit(127);
      case EACCES:
        fprintf(stderr, "pangolin: permission denied: %s\n", command);
        _exit(126);
      default:
        perror("pangolin");
        _exit(1);
      }

      // perror("execvp");
      _exit(EXIT_FAILURE);
    }
    check_dup_add_history(line);
    cout << "\n";
    ss.clear();
    last_status = 0;
  }
}

// 158, 72, 68
// 66, 63, 79
void REPL::shell_startup() {
  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    if (pw) home = pw->pw_dir;
  }
  const char *state = getenv("XDG_STATE_HOME");
  string state_dir;
  
  if (state) {
    state_dir = state;
  } else {
    state_dir = string(home) + "/.local/state";
  }
  string pangolin_dir = state_dir + "/pangolin";
  if (mkdir(state_dir.c_str(), 0700) == -1 && errno != EEXIST) {
    perror("mkdir state_dir");
  }

  if (mkdir(pangolin_dir.c_str(), 0700) == -1 && errno != EEXIST) {
    perror("mkdir pangolin_dir");
  }
  // cout << " \033[48;2;158;72;68m " << logo << "     \033[0m\n";
  string RED = "\033[38;2;158;72;68m";
  string BLUE = "\033[38;2;66;63;79m";
  string RESET = "\033[0m";
  string block = "█";
  vector<string> blues = {"╗", "║", "╝", "═", "╚", "╔", "░"};
  rl_catch_signals = 0;
  rl_catch_sigwinch = 0;
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
    cout << "\033[34m(pangolin)\033[32m " << buffer + 1 << "\033[34m$\033[0m ";
  }
}

bool REPL::is_built_in(string command) {
  for (size_t i = 0; i < builtin_commands.size(); i++) {
    if (builtin_commands[i] == command) {
      return true;
    };
  }
  return false;
}

bool REPL::is_aliased(string command) {
  for (auto c : aliases) {
    if (c.first == command) {
      return true;
    }
  }
  return false;
}

void REPL::check_dup_add_history(char *command) {
  HIST_ENTRY *entry = history_get(history_length);
  if (entry && strcmp(entry->line, command) == 0) {
    return;
  }
  add_history(command);
  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    if (pw) home = pw->pw_dir;
  }
  const char *state = getenv("XDG_STATE_HOME");
  string state_dir;

  if (state) {
    state_dir = state;
  } else {
    state_dir = string(home) + "/.local/state";
  }

  string history_path = state_dir + "/pangolin/history";

  ofstream fout(history_path, ios_base::app);
  if (!fout.good()) {
    perror("fout");
  }
  fout << command << "\n";
  fout.close();
}

void REPL::setup_signals() {
  struct sigaction sa{};
  sa.sa_handler = handle_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sa, nullptr);
}

void REPL::init_readline() {
  rl_catch_signals = 0;
  rl_catch_sigwinch = 0;
}

bool REPL::unknown_command_contains_slash(const string &command) {
  return command.find('/') != string::npos;
}

bool REPL::found_in_path(const string &command) {
  const char *path = getenv("PATH");
  if (!path) return false;

  stringstream ss(path);
  string dir;
  while (getline(ss, dir, ':')) {
    string full = dir + "/" + command;
    if (access(full.c_str(), X_OK) == 0) {
      return true;
    }
  }
  return false;
}

// TODO:
// ADD IN CLOCK STUFF FOR EXIT AND START UP, MAYBE MAKE AN DIGITAL CLOCK, print time of command like current shell on far right
// ADD IN CD AND EXIT - cd done, exit will take 2 seconds
// ADD IN ALIAS COMMAND
// figure out ctrl+c to not exit the shell but can still kill a process - done
// error messages
// piping
// implement ~ translation