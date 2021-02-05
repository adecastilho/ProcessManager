// CSC 360 Fall 2020
// Amanda de Castilho
// helper functions for the PMan program

#include <unistd.h>             // fork(), execvp()
#include <stdio.h>              // fprintf(), perror()
#include <stdlib.h>             // malloc()
#include <string.h>             // strtok()
#include <sys/types.h>          // pid_t 
#include <sys/wait.h>           // waitpid()
#include <signal.h>             // kill(), SIGTERM, SIGKILL, SIGSTOP, SIGCONT
#include <errno.h>              // errno
#include <readline/readline.h>  // readline()
#include <readline/history.h>   // readline()
#include "LinkedList.h"         // Linked List data structure

#define TRUE 1
#define FALSE 0
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define ACTIVE 0
#define STOPPED 1
#define INVALID_PID -1
#define MAX_ARGS 128
#define MAX_PATH_LEN 64
#define BUFFER 512
#define BASE_10 10
#define PROMPT "PMan: > "
#define INVAL_PID 1
#define INVAL_ARG_CNT 2
#define INVAL_CMD 3

void bg_entry(char **argv, Node **head);
void bglist_entry(Node **head); 
void bgsig_entry(pid_t pid, char *cmd, Node **head);
void pstat_entry(pid_t pid, Node *head);
void usage_pman(char *cmd, int error);
void check_zombieProcess(Node **head);
int is_whitespace(char *cmd);
char * get_input(char *p);
int cmd_req_pid(char *cmd);
int valid_pid(char **argv, Node *head);
int valid_argCount(char *cmd, int arg_count);
