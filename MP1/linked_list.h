#ifndef _linked_list_hpp_                   // check if the Header is already define
#define _linked_list_hpp_

//The node of linked list
struct node {
    //Pointer to the next node
    struct node *next;
    
    //Payload
    int key; // Key of the node
    int value_lenght; // Length of the the node
};

// initializer of the linked list
void   Init (int M, int b);

//	Delete the linked list get the space back
void   Destroy ();

// inserts the key and copies the value to the payload
int    Insert (int key,char * value_ptr, int value_len); 

// delete the node containing the key. Delete the first one if multiple node share the same key
int    Delete (int key); 

//	Looks up the first node with the key. Returns the pointer of the node
char*  Lookup (int key);

// Prints to the screen the entire linked list with the keys and values' length
void   PrintList ();		

#endif 