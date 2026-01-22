#include "../include/pangolin.h"

int main(){
  REPL repl;
  repl.init_readline();
  repl.setup_signals();
  repl.repl2();
  return 0;
}