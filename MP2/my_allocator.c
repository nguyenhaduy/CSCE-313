/*
    File: my_allocator.c

    Author: Duy Nguyen
            NealPatel
            Department of Computer Science
            Texas A&M University
    Date  : Sep 13th 2016

    Modified:

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

    Header** FREE_LIST; //Array of Header pointer

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    unsigned int NUM_LISTS;
    Addr START_MEMORY;
    const unsigned int HEADER_SIZE = 32;
    unsigned int MEMORY_LENGTH;
    unsigned int BASIC_BLOCK_SIZE;
    unsigned int BASIC_BASE; // = log2 (BASIC_BLOCK_SIZE)
    int count = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

void printList() {
  int counter = 0;
  Header* Curr_Header;
  Header* temp;
  for(int i = 0; i < NUM_LISTS; i++) {
    // Traverse through each free list
    Curr_Header = FREE_LIST[i];
    int count = 0;

    // If header isn't null, then traverse the linked list to get number of empty blocks
    if(Curr_Header != NULL) {
      temp = Curr_Header;
      while(temp != NULL) {
        count++;
        temp = temp->next;
      }
    }
    printf("FREE_LIST[%i] \t", i);
    int block_size = pow(2, i) * 128;
    printf("blocksize: %i bytes, number of free blocks: %i\n", block_size, count);
  }
}

int slipt_block(int i){
  int check_condition;
  if (i >= NUM_LISTS){
    printf("\nOut of Memory!!!\n");
    return 1;
  }

  //Recursive slipting the blocks if there is no free block in the memory
  if ((FREE_LIST[i] == NULL)&&(i < NUM_LISTS)){
    check_condition = slipt_block(i+1);
  }

  //Error handling, do not splipt if the free list is empty, non empty block
  if (FREE_LIST[i] != NULL){
	  Header* Curr_Header = FREE_LIST[i];

    //Calculate the buddy address base on bit wise xor
	  unsigned long long block_size = (unsigned long long)pow(2,(log2(BASIC_BLOCK_SIZE))+i-1);
    unsigned long long offset = (unsigned long long)((char*)Curr_Header - (char*)START_MEMORY);
    unsigned long long buddy_offset = offset ^ block_size;
    char* buddy = (char*)START_MEMORY + buddy_offset;

    //Take the current block out of the memory list
    FREE_LIST[i] = Curr_Header->next;
    if (Curr_Header->next != NULL){
      Curr_Header->next->prev = NULL;
    } 

    //Making the new header at the buddy address and set the attributes
    Header* New_Header = (Header*)buddy;
      New_Header->empty = 1;
      New_Header->next = FREE_LIST[i-1];
      New_Header->prev = Curr_Header;
      New_Header->size = i-1;

    //Put the new block in the freelist
    if(FREE_LIST[i-1] != NULL){
      FREE_LIST[i-1]->prev = New_Header;
    }
    
    //Set the attribute for current header
    Curr_Header->empty = 1;
    Curr_Header->next = New_Header;
    Curr_Header->prev = NULL;
    Curr_Header->size = i-1;
    FREE_LIST[i-1] =  Curr_Header;
  
   return 0;
  }
  else{
    return 1;
  }
  // printList();
}

void merge_block(Addr _a){
	
  Header* Curr_Header = (Header*) _a;

  //Calculate the buddy address base on bit wise xor
  unsigned long long offset = (long)((char*)Curr_Header - (char*)START_MEMORY);
  int list_location = ((Header*)Curr_Header)->size;
  unsigned long long block_size = (unsigned long long)pow(2,(log2(BASIC_BLOCK_SIZE)) +list_location);
  unsigned long long buddy_offset = offset ^ block_size;
  char* buddy = (char*)START_MEMORY + buddy_offset;
  Header* Buddy_Header = (Header*)buddy;

  //Only merge if the buddy is empty and the same size with the current block
  if ((Buddy_Header->empty == 1)&&(Buddy_Header->size == Curr_Header->size)){
    //get the current header out of the free list
    FREE_LIST[list_location] = Curr_Header->next;
    if(Curr_Header->next != NULL){
      Curr_Header->next->prev = NULL;
    }
    
    //get the buddy header out of the free list
    if (Buddy_Header->prev == NULL){
      FREE_LIST[list_location] = Buddy_Header->next;
      if (Buddy_Header->next != NULL) {
        Buddy_Header->next->prev = NULL;
      } 
    } else {
      Buddy_Header->prev->next = Buddy_Header->next;
      if (Buddy_Header->next != NULL) {
        Buddy_Header->next->prev = Buddy_Header->prev;
      } 
    }

    //determine wich is the smaller address between curr_header and buddy_header
    Addr small_addr;
    if (Buddy_Header < Curr_Header){
      small_addr = (Addr)Buddy_Header;
    } else {
      small_addr = (Addr)Curr_Header;
    }

    //Creating the Merged_Header and put it in front of the free list
    Header* Merged_Header= (Header*)small_addr;

    if (FREE_LIST[list_location+1] != NULL){
      FREE_LIST[list_location+1]->prev = Merged_Header;
    }

    //Set the attributes of the small header
      Merged_Header->empty = 1;
      Merged_Header->next = FREE_LIST[list_location + 1];
      Merged_Header->prev = NULL;
      Merged_Header->size = list_location + 1;

    //put the small header in front of the FREE_LIST
    FREE_LIST[list_location + 1] = Merged_Header;
    // printf("\nMerging !!!!\n");

    //Recursive call merge block
    if (list_location < NUM_LISTS){
      merge_block(small_addr);
    }  
  }

  // printList();
}


/* Don't forget to implement "init_allocator" and "release_allocator"! */

extern unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length){ 
  
  //Setting up the global variables
  MEMORY_LENGTH = _length;
  BASIC_BLOCK_SIZE = _basic_block_size;
  BASIC_BASE = (int) log2(BASIC_BLOCK_SIZE);
  NUM_LISTS = log2(MEMORY_LENGTH / BASIC_BLOCK_SIZE) + 1;

  // create the memory
  START_MEMORY = malloc(MEMORY_LENGTH);

  // malloc the array of freelist
  FREE_LIST = (Header**)malloc(NUM_LISTS * sizeof(Header*));
  
  // set FREE_LIST entries to NULL
  for (int i = 0; i < NUM_LISTS; ++i){
    FREE_LIST[i] = NULL; //start node
  }
  
  // create the first Header
  Header* temp_header = (Header*)START_MEMORY;
    temp_header->empty = 1;
    temp_header->next = NULL;
    temp_header->prev = NULL;
    temp_header->size = NUM_LISTS - 1;
  
  // store it in the first freelist to initialized the first free block
  FREE_LIST[NUM_LISTS - 1] = temp_header;

  printf("\nSuccessfully Initialized Memory!!!\n");

  printList();
    
  // return bytes available
  return (MEMORY_LENGTH - HEADER_SIZE);
}

extern Addr my_malloc(unsigned int _length) {
  /* This function return an address of a empty memory block to use.
     Call the slipt_block function incase there is no free block.
  */
    
  printf("\n\nMemory Allocating: [%d bytes]\n\n",_length);
  
  // ++count;
  // printf ("\n count = %i\n", count);

  //Calculate the minimum size of block that can fit the data and which free list it belongs to
  int index;
  if ( _length + HEADER_SIZE < BASIC_BLOCK_SIZE){
    index = 0;
  } else {
    index = log2((_length + HEADER_SIZE)/BASIC_BLOCK_SIZE) + 2;
  }

  //If there no free block in the free list, call slipt_block() to make one
  if ((FREE_LIST[index] == NULL)) {
    slipt_block(index+1);
  }

  //If there still are free memory block
  if(FREE_LIST[index] != NULL){
    Header* Curr_Header = FREE_LIST[index];
    FREE_LIST[index] = Curr_Header->next;
      Curr_Header->empty = 0;
      Curr_Header->next = NULL;
      Curr_Header->prev = NULL;
      Curr_Header->size = index;

  Addr my_addr = (Addr)((char*)Curr_Header + HEADER_SIZE);//allocated address does not include header
  return my_addr;
  } else {
    printf("\nOut of Memory!!!\n"); //Error handling in case there is no more free memory block
    return 0;
  }

  //printList();
}

extern int my_free(Addr _a) {
  /* This function return the allocated block to the freelist.
     Call the merge_block function to merge the blocks
  */

  printf("\nFreeing Memory From %llu!!!\n", _a);

  //Get Header address from the parameter
  char* Header_addr = (char*)(_a) - HEADER_SIZE;
  Header* New_Header = (Header*)Header_addr;
  int list_location = (int)New_Header->size;

  //Setting attribute for the new header
    New_Header->empty = 1;
    New_Header->next = FREE_LIST[list_location];
    New_Header->prev = NULL;
    New_Header->size = list_location;

  //Put the new header at the top of the free list
  FREE_LIST[list_location] = New_Header;

  //Call merge_block
  merge_block(Header_addr);

  //printList();
}

int release_allocator(){
  /* This function returns any allocated memory to the operating system.
       After this function is called, any allocation fails.
  */
  free(START_MEMORY);
  printf("Successfully Deallocated Memory\n");
  return 0;
}