// CSC 360 Fall 2020
// Amanda de Castilho
// Linked List

// The Linked List methods here were adapted from the following source: https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm

#include "LinkedList.h"

int remove_node(Node **head, pid_t pid) {
  Node *current = *head;
  Node *previous = NULL;
  
  
  if (*head == NULL){
    return -1;
  }
  
  while (current->pid != pid) {
    if(current->next == NULL){
      return -1;
    }
    else{
      previous = current;
      current = current->next;
    }
  }
  
  if (current == *head){
    *head = current->next;
  }
  else{
    previous->next = current->next;
    free(current);
  }
  return 0;
}

int add_node(Node **head, pid_t pid, char *fname, int status) {
  Node *new_node = (Node *) malloc(sizeof(Node));
  new_node->pid = pid;
  new_node->fname = fname;
  new_node->next = *head;
  new_node->status = status;
  *head = new_node;
  
  return 0;
}

Node* find_node(Node *head, pid_t pid) {
  Node *current = head;
  
  if (head == NULL){
    return NULL;
  }
  
  
  while (current->pid != pid){
    if (current->next == NULL) {
      return NULL;
    }    
    else{
      current = current->next;
    }
  }  
  return current;
}

int length(Node *head) {
   int length = 0;
   Node *current;
	
   for(current = head; current != NULL; current = current->next) {
      length++;
   }
	
   return length;
}

int get_status(Node *head, pid_t pid){

  Node *node = find_node(head, pid);
  
  if (node == NULL) return -1;
  
  return node->status;
}

int update_status(Node *head, pid_t pid, int status){
  Node *node = find_node(head, pid);
  
  if (node == NULL) return -1;
  
  node->status = status;
  return 0;
}