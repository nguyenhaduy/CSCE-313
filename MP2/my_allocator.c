/*
    File: my_allocator.c

    Author: Duy Nguyen
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

    Header** FREE_LIST;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    unsigned int NUM_LISTS;
    Addr START_MEMORY;
    Addr END_MEMORY;
    const unsigned int HEADER_SIZE = 13;
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

void check_block(){
	
}

int slipt_block(int i){
  int check_condition;
  if (i < 1){
    return 1;
  }
  //Recursive
  printf("\nCheck recursive %i\n",i);
  if ((FREE_LIST[i] == NULL)&&(i > 0)){
    check_condition = slipt_block(i+1);
  }


  if (FREE_LIST[i] != NULL){
    printf("\nCheck inside\n");
	  Header* Curr_Header = FREE_LIST[i];
    // uintptr_t temp_addr = (uintptr_t)Curr_Header;
    // printf("\ntemp_addr = %i\n", temp_addr);

    // temp_addr = temp_addr ^ (int)pow(2, i -1 + 2);
    // printf("\ntemp_addr = %i\n", temp_addr);

    // printf("\nBASIC_BLOCK_SIZE + i-1 = %i\n", log2(BASIC_BLOCK_SIZE) + i);

    unsigned long long block_size = (unsigned long long)pow(2,(log2(BASIC_BLOCK_SIZE))+i);
    printf("\nblock_size = %i\n", block_size);

    // char* buddy =(char*)(( (unsigned long long) Curr_Header) ^ block_size);
    // printf("\nCurr_Header = %i\n", Curr_Header);
    // printf("\nbuddy = %i\n", buddy);

    int offset = (int)((char*)Curr_Header - (char*)START_MEMORY);
    int buddy_offset = offset ^ block_size;
    char* buddy = (char*)START_MEMORY + buddy_offset;

    printf("\nCurr_Header = %i\n", Curr_Header);
    printf("\nbuddy = %i\n", buddy);


    // temp_addr = (uintptr_t)Curr_Header;
    printf("\nCheck inside2\n");
    Header* New_Header = (Header*)buddy;
      New_Header->empty = 1;
      New_Header->next = FREE_LIST[i-1];
      New_Header->prev = Curr_Header;
      New_Header->size = i-1;

    printf("\n size = %i\n", New_Header->size);
    FREE_LIST[i] = Curr_Header->next;
    Curr_Header->next = New_Header;
    FREE_LIST[i-1] =  Curr_Header;
  
   return 0;
  }
  else{
    return 1;
  }
}

void merge_block(Addr _a){
	
  Header* Curr_Header = (Header*) _a;

  int offset = (int)((char*)Curr_Header - (char*)START_MEMORY);
  int list_location = ((Header*)Curr_Header)->size;
  unsigned long long block_size = (unsigned long long)pow(2,(log2(BASIC_BLOCK_SIZE))+list_location-1);
  int buddy_offset = offset ^ block_size;
  char* buddy = (char*)START_MEMORY + buddy_offset;
  Header* Buddy_Header = (Header*)buddy;

  if (Buddy_Header->empty == 1){
    Header* min_addr;
    if (Buddy_Header < Curr_Header){
      min_addr = Buddy_Header;
    } else {
      min_addr = Curr_Header;
    }

  }

}

void remove_header (int i){

}

void check_header(){

}

/* Don't forget to implement "init_allocator" and "release_allocator"! */

extern unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length){ 
  // determine sizes to be used
  MEMORY_LENGTH = _length;
  BASIC_BLOCK_SIZE = _basic_block_size;
  BASIC_BASE = (int) log2(BASIC_BLOCK_SIZE);
  NUM_LISTS = log2(MEMORY_LENGTH / BASIC_BLOCK_SIZE) + 1;
  printf ("\nNUM_LISTS = %i\n", NUM_LISTS);

  // create the memory
  START_MEMORY = malloc(MEMORY_LENGTH);
  // END_MEMORY = START_MEMORY - MEMORY_LENGTH;
  
  // create the freelist
  // 1. malloc the first dimension
  FREE_LIST = (Header**)malloc(NUM_LISTS * sizeof(Header*));
  
  // // 3. set FREE_LIST entries to NULL
  for (int i = 0; i < NUM_LISTS; ++i){
    FREE_LIST[i] = NULL; //start node
  }
  // printf("check2\n");
  
  // create the first Header
  Header* temp_header = (Header*)START_MEMORY;
    temp_header->empty = 1;
    temp_header->next = NULL;
    temp_header->prev = NULL;
    temp_header->size = NUM_LISTS - 1;
  
  // store it in the first freelist to initialized the first free block
  FREE_LIST[NUM_LISTS - 1] = temp_header;

  printf("\nSuccessfully Initialized Memory!!!\n");
    
  // return bytes available
  return (MEMORY_LENGTH - HEADER_SIZE);
}

extern Addr my_malloc(unsigned int _length) {
  /* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
  */
    
  printf("\n\n Memory Allocating: [%d]\n\n",_length);
  ++count;

  printf ("\n count = %i\n", count);
  int index;
  if ( _length + HEADER_SIZE < BASIC_BLOCK_SIZE){
    index = 0;
  } else {
    index = log2((_length + HEADER_SIZE)/BASIC_BLOCK_SIZE) + 2;
  }
  int i = index;
  printf("\n BASIC_BLOCK_SIZE = %i \n", BASIC_BLOCK_SIZE);
  printf("\n _length = %i \n",_length);
  printf("\n HEADER_SIZE = %i \n",HEADER_SIZE);



  printf("\n index = %i \n",index);

  if ((FREE_LIST[index] == NULL)) {
    slipt_block(index+1);
    // ++i;
    printf("\nCheck %i", i);
  }

  printf("\nCheck done 1\n");

  printf("\n index = %i \n",index);
  Header* Curr_Header = FREE_LIST[index];
  printf("\nCheck done 2\n");
  FREE_LIST[index] = Curr_Header->next;
  printf("\nCheck done 3\n");
    Curr_Header->empty = 0;
    Curr_Header->next = NULL;
    Curr_Header->prev = NULL;

  Addr my_addr = (char*)Curr_Header + HEADER_SIZE+1;//give address to free mem do not include header

  //remove_header();
  return my_addr;

}

extern int my_free(Addr _a) {
  /* Same here! */
  
  char* Header_addr = (char*)_a - HEADER_SIZE-1;
  Header* New_Header = (Header*)Header_addr;
  int list_location = New_Header->size;
  printf("\nLocation = %i\n", list_location);
  printf("\nNext = %i\n", New_Header->next );

      New_Header->empty = 1;
      New_Header->next = FREE_LIST[list_location];
      New_Header->prev = NULL;
      New_Header->size = list_location;
  FREE_LIST[list_location] = New_Header;

  merge_block(_a);

}

int release_allocator(){
  /* This function returns any allocated memory to the operating system.
       After this function is called, any allocation fails.
    */
  free(START_MEMORY);
  printf("Successfully Deallocated Memory\n");
  return 0;
}