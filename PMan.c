// CSC 360 Fall 2020
// Amanda de Castilho
// PMan

#include "PMan.h"

// Process list data structure
Node *headPnode = NULL;

int main() {

  while(TRUE) { 
  
    // Get user input
    char *input = get_input(PROMPT);
    
    // Skip loop if no input (prevents segfault when user enters whitespace)
    if (!input){
      check_zombieProcess(&headPnode);
      continue;
    }
    
    // Parse input
    char *cmd = strtok(input, " ");
    char *argv[MAX_ARGS];
    int arg_count = 0;
    char *token = strtok(NULL, " ");
    while (token){
      argv[arg_count++] = token;
      token = strtok(NULL, " ");
    } 
    argv[arg_count] = NULL;
    
    // Check arg count
    if (!valid_argCount(cmd, arg_count)){
      usage_pman(cmd, INVAL_ARG_CNT);
      check_zombieProcess(&headPnode);
      continue;
    }
    
    // Check pid validity
    int pid = 0;
    if (cmd_req_pid(cmd)){
      pid = valid_pid(argv, headPnode);
      if (!pid){
        usage_pman(cmd, INVAL_PID);
        check_zombieProcess(&headPnode);
        continue;
      }
    }
    
    // Process cmd
    if (!strcmp(cmd, "bg")){
      bg_entry(argv, &headPnode);
    }
    else if(!strcmp(cmd,"bglist")){
      bglist_entry(&headPnode);
    }
    else if(!strcmp(cmd,"bgkill") || !strcmp(cmd,"bgstop") || !strcmp(cmd,"bgstart")){
      bgsig_entry(pid, cmd, &headPnode);
    }
    else if(!strcmp(cmd, "pstat")){
      pstat_entry(pid, headPnode);
    }
    else{
      usage_pman(cmd, INVAL_CMD);
    }
    
    // Zombie check
    check_zombieProcess(&headPnode);
  }
}

    