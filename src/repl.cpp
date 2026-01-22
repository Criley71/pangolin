#include "../include/repl.h"

volatile sig_atomic_t sigint_recieved = 0;

void handle_sigint(int) {
  sigint_recieved = 1;

  rl_replace_line("", 0);
  rl_crlf();
  rl_on_new_line();
  rl_redisplay();
}

void REPL::repl2() {
  shell_startup();
  Commands Command;
  char cwd[2048];
  Lexer lexer;
  Executor executor;

  while (true) {
    sigint_recieved = 0;
    if (!getcwd(cwd, sizeof(cwd))) {
      perror("getcwd");
      continue;
    }

    string prompt = "\033[34m(pangolin)\033[32m" + string(cwd) + "\033[34m$\033[0m ";

    char *input = readline(prompt.c_str());
    if (!input) {
      break; // ctrl+d
    }
    if (sigint_recieved) {
      free(input);
      continue;
    }

    

    try {
      string str_input = input;
      auto tokens = lexer.lex_input(str_input);
      Parser parser(tokens);
      auto ast = parser.parse();
      executor.execute(ast.get());
    } catch (const exception &e) {
      cerr << "?" << e.what() << '\n';
    }
    check_dup_add_history(input);
  }
}

void REPL::repl_loop() {
  shell_startup();

  Commands Command;
  char cwd[2048];
  int last_status = 0;

  while (true) {
    sigint_recieved = 0;

    if (!getcwd(cwd, sizeof(cwd))) {
      perror("getcwd");
      continue;
    }

    string prompt = "\033[34m(pangolin)\033[32m" + string(cwd) + "\033[34m$\033[0m ";

    char *line = readline(prompt.c_str());
    if (!line) {
      break; // ctrl+d
    }

    if (sigint_recieved) {
      free(line);
      continue;
    }

    vector<vector<string>> commands;
    commands.push_back({});

    stringstream ss(line);
    string token;
    while (ss >> token) {
      if (token == "&&") {
        commands.push_back({});
      } else {
        commands.back().push_back(token);
      }
    }

    for (auto &cmd : commands) {
      for (auto &arg : cmd) {
        arg = tilde_translation(arg);
      }
    }

    if (!commands.empty() &&
        !commands[0].empty() &&
        commands[0][0] == "exit") {
      free(line);
      exit(EXIT_SUCCESS);
    }

    last_status = 0;

    for (auto &cmd : commands) {
      if (cmd.empty()) {
        continue;
      }

      if (is_built_in(cmd[0])) {
        Command.determine_command(cmd);
      }

      if (is_aliased(cmd[0])) {
        for (auto &c : aliases[cmd[0]]) {
          cmd.push_back((const_cast<char *>(c.c_str())));
        }
      }

      if (!unknown_command_contains_slash(cmd[0]) &&
          !found_in_path(cmd[0])) {
        cerr << "pangolin: command not found: " << cmd[0] << "\n";
        last_status = 127;
        break;
      }

      vector<char *> argv;
      for (auto &s : cmd) {
        argv.push_back(const_cast<char *>(s.c_str()));
      }
      argv.push_back(nullptr);

      pid_t pid = fork();
      if (pid < 0) {
        perror("fork");
        last_status = 1;
        break;
      }

      if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        execvp(argv[0], argv.data());

        switch (errno) {
        case ENOENT:
          _exit(127);
        case EACCES:
          _exit(126);
        default:
          _exit(1);
        }
      }

      int status;
      waitpid(pid, &status, 0);

      if (WIFEXITED(status)) {
        last_status = WEXITSTATUS(status);
      } else {
        last_status = 1;
      }

      if (last_status != 0) {
        break;
      }
    }

    check_dup_add_history(line);

    free(line);
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

  load_history();
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
  setenv("TERM", "xterm-256color", 1);
  setenv("LS_COLORS", "di=34:fi=0:ln=36:ex=32", 1);
  //cout << isatty(STDOUT_FILENO) << "\n";
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

string REPL::tilde_translation(string arg) {
  if (arg.empty() || arg[0] != '~') {
    return arg;
  }

  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    if (!pw) return arg;
    home = pw->pw_dir;
  }

  if (arg.size() == 1) {
    return string(home);
  }

  if (arg[1] == '/') {
    return string(home) + arg.substr(1);
  }

  return arg;
}

void REPL::load_history() {
  ifstream fin(get_history_dir());
  if (!fin.good()) {
    perror("history file open error");
  }
  stack<string> history;
  string buffer;
  while (getline(fin, buffer)) {
    history.push(buffer);
  }
  while (!history.empty()) {
    add_history((history.top()).c_str());
    history.pop();
  }
}



string REPL::get_history_dir() {
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

  return state_dir + "/pangolin/history";
}


// TODO:
// ADD IN CLOCK STUFF FOR EXIT AND START UP, MAYBE MAKE AN DIGITAL CLOCK, print time of command like current shell on far right
// ADD IN CD AND EXIT - cd done, exit will take 2 seconds
// ADD IN ALIAS COMMAND
// figure out ctrl+c to not exit the shell but can still kill a process - done
// error messages - done?
// piping
// implement ~ translation - done
// && implementation
