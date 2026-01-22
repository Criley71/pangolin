#include "../include/commands.h"


bool Commands::determine_command(vector<string>& command){
  if(command[0] == "cd"){
    cd(command[1]);
    return true;
  }
  return false;
}


void Commands::cd(string path){
  int rc = chdir(path.c_str());
  if(rc < 0){
    perror("cd failed idk change this error message");
  }
}

void Commands::exit_shell(){
  
}