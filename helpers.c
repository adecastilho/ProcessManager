// CSC 360 Fall 2020
// Amanda de Castilho
// helper functions for the PMan program

#include "helpers.h"

// Function to get user input
// returns NULL if input is whitespace/empty line
char * get_input(char *p){
  char * retVal = readline(p);
  if (strcmp(retVal, "") == 0 || is_whitespace(retVal)){
    retVal = NULL;
  } 
  return retVal;
}

// Function to check if cmd string is all whitespace
int is_whitespace(char *cmd){
  char c = cmd[0];
  int index = 0;
  int retVal = TRUE;
  while (c != '\0'){
    if (c != ' '){
      retVal = FALSE;
      break;
    }
    c = cmd[++index];
  }
  return retVal;
}

// Function to check if given cmd requires a pid argument
int cmd_req_pid(char *cmd){
  if (!strcmp(cmd,"bgkill") || !strcmp(cmd,"bgstop") || !strcmp(cmd,"bgstart") || !strcmp(cmd,"pstat")) return TRUE;
  else return FALSE;
}

// Confirms pid argument is valid (exists, is a number, corresponds to a current bg process)
// returns FALSE if not valid, else returns the pid value from the argv list
int valid_pid(char **argv, Node *head){
  if (argv[0] == NULL) return FALSE;
  int pid = atoi(argv[0]);
  if (pid == 0) return FALSE;
  if (!find_node(head, pid)) return FALSE;
  else return pid;
}

// Confirms the correct number of arguments are provided for the given cmd
int valid_argCount(char *cmd, int arg_count){
  if (cmd_req_pid(cmd) && arg_count != 1){
    return FALSE;
  }
  else if(!strcmp(cmd,"bglist") && arg_count != 0){
    return FALSE;
  }
  else if(!strcmp(cmd,"bg") && arg_count ==0){
    return FALSE;
  }
  return TRUE;
}

// Function to process bad user input
void usage_pman(char *cmd, int error) {
  if (error == INVAL_PID){
    fprintf(stderr, "%s: valid pid required\n", cmd);
  }
  else if (error == INVAL_ARG_CNT){
    if (cmd_req_pid(cmd)){
      fprintf(stderr, "%s: invalid arguments: 1 argument required\n", cmd);
    }
    else if (!strcmp(cmd,"bglist")){
      fprintf(stderr, "%s: invalid arguments: 0 arguments required\n", cmd);
    }
    else{
      fprintf(stderr, "%s: invalid arguments: program path required\n", cmd);
    }
  }
  else if (error == INVAL_CMD){
    fprintf(stderr, "%s: command not found\n", cmd);
  }
}

// Function to process bg cmd - starts a new background process
void bg_entry(char **argv, Node **head) {

  pid_t pid;
  pid = fork();
  
  if (pid == 0) { // Child process
  
    if (execvp(argv[0], argv) < 0) {
      perror("Error on execvp");
    }
    
    exit(EXIT_SUCCESS);
  }
  else if (pid > 0) { // Parent process
    add_node(head, pid, argv[0], ACTIVE);
    usleep(1000);
    check_zombieProcess(head);
  }
  else {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }
}

// Function to process bglist cmd - lists all background processes
void bglist_entry(Node **head) {

  // Check zombie's to remove from linked list prior to printout
  check_zombieProcess(head);
  
  Node * tmp = *head;
  while (tmp) {
    printf("%d:\t%s\n", tmp->pid, tmp->fname);
    tmp = tmp->next;
  }
  fprintf(stdout, "Total background jobs:\t%d\n", length(*head));
}

// Function to process bgkill, bgstop, and bgstart commands
void bgsig_entry(pid_t pid, char *cmd, Node **head) {

  // zombie check
  check_zombieProcess(head);
  
  // Confirm process still exists & get status
  int pid_status = get_status(*head, pid);
  if (pid_status == INVALID_PID){
    fprintf(stderr, "Error:\t Process %d does not exist.\n", pid);
    return;
  }
  
  if (!strcmp(cmd,"bgkill")){
    
    // If process is stopped, restart it before sending kill signal
    if (pid_status == STOPPED){
      kill(pid, SIGCONT);
    } 
    
    kill(pid, SIGTERM);
    remove_node(head, pid);
    fprintf(stderr, "Process %d was killed\n", pid);
    usleep(1000);
    
    return;
    
  }
  
  else if (!strcmp(cmd,"bgstop")){
  
    kill(pid, SIGSTOP);
    fprintf(stderr, "Process %d was stopped\n", pid);
    update_status(*head, pid, STOPPED);
    
  }
  
  else if (!strcmp(cmd,"bgstart")){
    kill(pid, SIGCONT);
    fprintf(stderr, "Process %d was started\n", pid);
    update_status(*head, pid, ACTIVE);
  }
  
}

// Function to process pstat cmd - lists status data about the given pid
void pstat_entry(pid_t pid, Node *head) {

  // confirm the given pid still exists
  if (!find_node(head, pid)){
    fprintf(stderr, "Error:\t Process %d does not exist.\n", pid);
    return;
  }
  
  // Create stat / status path strings
  char statPath [MAX_PATH_LEN];
  sprintf(statPath, "/proc/%d/stat", pid);
  char statusPath [MAX_PATH_LEN];
  sprintf(statusPath, "/proc/%d/status", pid);
  
  // Open the stat file, confirm success
  FILE *stat_ptr = fopen(statPath, "r");
  
  if (!stat_ptr){
    fprintf(stderr, "PMan: > Cannot access stat file for pid: %d\n", pid);
    return;
  }
  
  // Get string of data from the stat file pointer
  char stat [BUFFER];
  fgets(stat, BUFFER, stat_ptr);
  
  // Tokenize the stat data into an array
  char *stat_arr[MAX_ARGS];
  int index = 0;
  char *token = strtok(stat, " ");
  
  while (token){
    stat_arr[index++] = token;
    token = strtok(NULL, " ");
  }
  
  // Close the stat file
  fclose(stat_ptr);
  
  
  // Open status file, confirm succss
  FILE *status_ptr = fopen(statusPath, "r");
  
  if (!status_ptr){
    fprintf(stderr, "PMan: > Cannot access status file for pid: %d\n", pid);
    return;
  }
  
  // Iterate through strings in status to find context switch data
  char status_line [BUFFER] = "";
  
  char vctxts [BUFFER] = "";
  char nvctxts [BUFFER] = "";
  
  fgets(status_line, BUFFER, status_ptr);
  
  while (!strstr(status_line, "voluntary")){
    fgets(status_line, BUFFER, status_ptr);
  }
  
  strncat(vctxts, status_line, BUFFER);
  fgets(nvctxts, BUFFER, status_ptr);
  
  // Close the status file
  fclose(status_ptr);
  
  
  // Prepare pstat data as required
  float utime = ((float)atoi(stat_arr[13])) / (float)sysconf(_SC_CLK_TCK);
  float stime = ((float)atoi(stat_arr[14])) / (float)sysconf(_SC_CLK_TCK);
  long rss;
  char *ptr;
  rss = strtol(stat_arr[23], &ptr, BASE_10);
  
  // Print the pstat data for the given pid
  int width = 31;
  printf("%-*s %s\n", width, "comm: ", stat_arr[1]);
  printf("%-*s %s\n", width, "state: ", stat_arr[2]);
  printf("%-*s %f seconds\n", width, "utime: ", utime);
  printf("%-*s %f seconds\n", width, "stime: ", stime);
  printf("%-*s %ld\n", width, "rss: ", rss);
  printf("%s%s", vctxts, nvctxts);
  
}

// Function to check for and terminate zombie processes
void check_zombieProcess(Node **head){

	int status = 0;
	int retVal = 0;
	
	while(TRUE) {
		usleep(1000);

		if(*head == NULL){
			return ;
		}
		retVal = waitpid(-1, &status, WNOHANG);
		if(retVal > 0) {
   
			//remove the background process from data structure
      if (WIFSIGNALED(status)){
        // if the process was already removed from the linked list, continue
        if (!find_node(*head, retVal)){
          continue;
        }
        fprintf(stderr, "Process %d was killed\n", retVal);
      }
      else {
        fprintf(stderr, "Process %d exits\n", retVal);
      }
      remove_node(head, retVal);
		}
		else if(retVal == 0){
			break;
		}
		else{
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}
	}
	return;
}
