// CSC 360 Fall 2020
// Amanda de Castilho
// Linked List

#include <stdio.h>              // fprintf()
#include <stdlib.h>             // malloc()
#include <sys/types.h>          // pid_t 

typedef struct Node {
  char *fname;
  pid_t pid;
  int status;
  struct Node* next;
} Node;

int remove_node(Node **head, pid_t pid);

int add_node(Node **head, pid_t pid, char *fname, int status);

Node* find_node(Node *head, pid_t pid);

int length(Node *head);

int get_status(Node *head, pid_t pid);

int update_status(Node *head, pid_t pid, int status);
