#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"

//initialize the global varible
int memory_pool;
int node_size;
int number_of_nodes;
struct node *head_pointer;
char *free_pointer;

// allocate memory, given by M
void Init (int M, int b) {
    memory_pool = M;
    node_size = b;
    head_pointer = (node*)malloc(memory_pool);
    free_pointer = (char*)head_pointer;
}

void Destroy () {
    memory_pool = 0;
    node_size = 0;
    number_of_nodes = 0;
    head_pointer = NULL;
    free_pointer = NULL;
    free(head_pointer);
}

int Insert (int key, char * value_ptr, int value_len) {
    // Make sure having enough space for a new node
    if (memory_pool / node_size < number_of_nodes) {
        printf("ERROR: Not enough space for a new node!!!\n");
        return -1;
    }

    // Check if the value given is fit in the space
    if (value_len > node_size - (sizeof(key) + sizeof(value_len) + sizeof(free_pointer))) {
        printf("ERROR: Given value size is too big! Insertion aborted!!!\n");
        return -1;
    }

    // Make the new node at the free_pointer
    node *new_node = (node*)free_pointer;
    new_node->key = key;
    new_node->value_lenght = value_len;
    new_node->next = (node*)(free_pointer + node_size);

    memcpy((free_pointer + sizeof(node)), value_ptr, value_len);
    free_pointer = free_pointer + node_size;
    number_of_nodes++;
    return key;
}

int Delete (int key) {
    struct node* curr_node = head_pointer;
    struct node* prev_node = NULL;
    int deleted = 0;

    for (int i = 1; i <= number_of_nodes; i++) {
        if (curr_node->key == key && deleted != 1) {
            if (prev_node != NULL) {
                prev_node->next = curr_node->next;
            } else {
                head_pointer = curr_node->next;
            }
            number_of_nodes--;
            printf("\nNode with key %d deleted! \n", key);
            deleted = 1;
        }
        prev_node = curr_node;
        curr_node = curr_node->next;
    }

    if (deleted == 1) {
        return key;
    } else {
        printf("\nERROR: Cannot find a node with key %d!!!\n", key);
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
    return NULL;
}

void PrintList () {
    node* curr_node = head_pointer;
    for (int i = 0; i < number_of_nodes; ++i) {
        printf ("Key = %d, Value Len = %d\n", curr_node->key, curr_node->value_lenght);
        curr_node = curr_node->next;
    }
}