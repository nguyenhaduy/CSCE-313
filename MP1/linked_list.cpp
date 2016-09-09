#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"
#include <iostream>

//initialize the global varible
int memory_pool;
int node_size;
int number_of_nodes;
struct node *begin_pointer;
struct node *head_pointer;
struct node *tail_pointer;
char *free_pointer;

// allocate memory, given by M
void Init (int M, int b) {
    memory_pool = M;
    node_size = b;
    number_of_nodes = 0;
    begin_pointer = (node*)malloc(memory_pool);
    head_pointer = begin_pointer;
    tail_pointer = begin_pointer;
    free_pointer = (char*)head_pointer;    
    printf("\nSuccessfully create the memory!!\n");
}

void Destroy () {
    memory_pool = 0;
    node_size = 0;
    number_of_nodes = 0;
    head_pointer = NULL;
    tail_pointer = NULL;
    free_pointer = NULL;
    free(begin_pointer);
    begin_pointer = NULL;
    printf("\nSuccessfully free the memory!!\n");
}

int gap_check(){
    if (begin_pointer != head_pointer){     //if the first block is empty
        free_pointer = (char*)(begin_pointer);
    } else {    //search for the first empty spot in the memory block
        struct node* curr_node = head_pointer;
        char *check_pointer;
        int empty_check = 0;
        while (curr_node->next != NULL && empty_check != 1) {
            check_pointer = (char*)curr_node;
            check_pointer = check_pointer + node_size;
            if (check_pointer != (char*)(curr_node->next)){
                free_pointer = (char*)curr_node;
                free_pointer = free_pointer + node_size;
                empty_check = 1;
            }
            curr_node = curr_node->next;

        }
    }
};

int Insert (int key, char * value_ptr, int value_len) {
    // Make sure having enough space for a new node
    if (memory_pool / node_size < number_of_nodes) {
        printf("\nERROR: Not enough space for a new node!!!\n");
        return -1;
    }

    // Check if the value given is fit in the space
    if (value_len > node_size - (sizeof(key) + sizeof(value_len) + sizeof(begin_pointer))) {
        printf("\nERROR: Given value size is too big! Insertion aborted!!!\n");
        return -1;
    }
    // Make the new node at the free_pointer
    node *new_node = (node*)free_pointer;
    new_node->key = key;
    new_node->value_lenght = value_len;
    if ((char*)begin_pointer == free_pointer){     //if the first block is empty
        new_node->next = (node*)(free_pointer + node_size);
    }
    else{
        new_node->next = ((node*)(free_pointer - node_size))->next;
        ((node*)(free_pointer - node_size))->next = new_node;
    }

    memcpy((free_pointer + sizeof(node)), value_ptr, value_len);
    number_of_nodes++;

    //check condition for free_pointer and tail_pointer
    if (free_pointer == (char*)(tail_pointer)){        
        tail_pointer = tail_pointer->next;
        free_pointer = free_pointer + node_size;
    }
    else if (free_pointer == (char*)(begin_pointer)){
        head_pointer = begin_pointer;
        gap_check();
    }
    else{
        gap_check();
    }
    printf("\nSuccessfully insert data with key %d !!!\n", key);
    return key;
}

int Delete (int key) {
    struct node* curr_node = head_pointer;
    struct node* prev_node = NULL;
    int deleted_check = 0;

    for (int i = 0; i < number_of_nodes; ++i) {
        if (curr_node->key == key && deleted_check != 1) {
            if (prev_node != NULL) {
                prev_node->next = curr_node->next;
            } else {
                head_pointer = curr_node->next;
            }
            number_of_nodes--;
            printf("\nNode with key %d was deleted! \n", key);
            deleted_check = 1;
        }
        if (curr_node == tail_pointer){
            tail_pointer = prev_node;
        }
        
        //Delete the node
        prev_node = curr_node;
        curr_node = curr_node->next;
    }

    if (deleted_check == 1) {
        gap_check();
        return key;
    } else {
        printf("\nERROR: Cannot find a node with key %d to delete!!!\n", key);
        return -1;
    }
}

char* Lookup (int key) {
    node* curr_node = head_pointer;
    for (int i = 0; i < number_of_nodes; ++i) {
        if (curr_node->key == key) {
            char* location = (char*) curr_node + sizeof(node*);
            return location;
        }
        curr_node = curr_node->next;
    }
    printf("\nCannot find a node with key %d !\n", key);
    return NULL;
}

void PrintList () {
    node* curr_node = head_pointer;
    for (int i = 0; i < number_of_nodes; ++i) {
        printf ("Key = %d, Value Len = %d\n", curr_node->key, curr_node->value_lenght);
        curr_node = curr_node->next;
    }
}