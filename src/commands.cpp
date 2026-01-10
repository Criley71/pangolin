#include "../include/commands.h"


void Commands::determine_command(vector<string>& command){
  if(command[0] == "cd"){
    cd(command[1]);
  }
}


void Commands::cd(string path){
  int rc = chdir(path.c_str());
  if(rc < 0){
    perror("cd failed idk change this error message");
  }
}

void Commands::exit_shell(){
  
}