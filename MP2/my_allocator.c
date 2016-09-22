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

    int NUM_LISTS;
    Addr START_MEMORY;
    Addr END_MEMORY;
    const unsigned int HEADER_SIZE = 13;
    unsigned int MEMORY_LENGTH;
    unsigned int BASIC_BLOCK_SIZE;
    // Header** LIST_PTR;
    // int LIST_SIZE;
    // int BASE;
    // uintptr_t MEM_BEGIN;//pointer to beginning of memory block for bit calculations

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
  if (i >= NUM_LISTS){
    return 1;
  }

  printf("\nCheck recursive %i\n",i);
  if (FREE_LIST[i] == NULL){
    check_condition = slipt_block(i+1);
  }


  if ((FREE_LIST[i] != NULL)&&(check_condition == 0)){
	  Header* Curr_Header = FREE_LIST[i+1];
    uintptr_t temp_addr = (uintptr_t)Curr_Header;
    temp_addr = temp_addr ^ (int)pow(2, BASIC_BLOCK_SIZE + i);
    Header* New_Header = (Header*)temp_addr;
      New_Header->empty = 1;
      New_Header->next = FREE_LIST[i];
      New_Header->prev = Curr_Header;
      New_Header->size = (BASIC_BLOCK_SIZE + i);
    FREE_LIST[i] = Curr_Header->next;
    Curr_Header->next = New_Header;
    FREE_LIST[i] =  Curr_Header;
  
   return 0;
  }
  else{
    return 1;
  }
}

void remove_header (int i){

}

void merge_block(){
	
}

void check_header(){

}

// int getList(Addr _a) {
//   Header *a = (Header*)_a;
  
//   int i = NUM_LISTS, j = a->size + HEADER_SIZE;
//   while (j != MEMORY_LENGTH/(int)pow(2,i)) {
//     i --;
//     if (i < 0) {
//       printf("ERROR: GETLIST FAILED ON %i!",a->size + HEADER_SIZE);
//       exit(EXIT_FAILURE);
//     }
//   }
  
//   return NUM_LISTS-i-1;
// }

// extern void print() {
//   int i, j;
//   Header *h = NULL;
  
//   printf("FREE_LIST:\n");
//   for (i = 0; i < NUM_LISTS; ++i) {
  
//     printf("%i: ",i);
//     for (j = 0; j < 2; ++j) {
//       if (FREE_LIST[i][j] == NULL) {
//         printf("[ ]");
//       } else {
//         h = (Header*)FREE_LIST[i][j];
//         printf("[%i]",h->size + HEADER_SIZE);
//       }
//     }
    
//     printf("\n");
//   }
  
//   printf("MEMORY: \n");
//   h = (Header*)START_MEMORY;
  
//   int oC = 0;
//   while(h != NULL && oC < 100) {
//     printf("[");
//     if (h->prev != NULL) printf("%i<-", h->prev->size + HEADER_SIZE);
//     printf("(%i,%i)",h->size + HEADER_SIZE, h->empty);
//     if (h->next != NULL) printf("->%i", h->next->size + HEADER_SIZE);
//     printf("]\t");
    
//     h = h->next;
//     oC ++;
//   }
//   printf("\n\n\n");
// }

/* Don't forget to implement "init_allocator" and "release_allocator"! */

extern unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length){ 
  // determine sizes to be used
  MEMORY_LENGTH = _length;
  BASIC_BLOCK_SIZE = _basic_block_size;
  NUM_LISTS = log2(MEMORY_LENGTH / BASIC_BLOCK_SIZE) + 1;
    
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
    temp_header->size = (MEMORY_LENGTH - HEADER_SIZE);
  
  // store it in the first freelist to initialized the first free block
  FREE_LIST[NUM_LISTS - 1] = temp_header;

  printf("\nSuccessfully Initialized Memory!!!\n");
    
  // return bytes available
  return temp_header->size;
}

extern Addr my_malloc(unsigned int _length) {
  /* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
  */
  // Header* temp_header = NULL;
  // Header* to_use = NULL;
  // Header* tNext = NULL;
    
  printf("\n\n Memory Allocating: [%d]\n\n",_length);
  
  int index = log2(_length + HEADER_SIZE)/BASIC_BLOCK_SIZE;
  int i = index;
  printf("\n BASIC_BLOCK_SIZE = %i \n", BASIC_BLOCK_SIZE);
  printf("\n _length = %i \n",_length);
  printf("\n HEADER_SIZE = %i \n",HEADER_SIZE);



  printf("\n index = %i \n",index);

  if ((FREE_LIST[index] == NULL)) {
    slipt_block(i);
    // ++i;
    printf("\nCheck %i", i);
  }

  printf("\nCheck done\n");

  Header* Curr_Header = FREE_LIST[index];
  printf("\nCheck done\n");
  FREE_LIST[index] = Curr_Header->next;
  printf("\nCheck done\n");
    Curr_Header->empty = 0;
    Curr_Header->next = NULL;
    Curr_Header->prev = NULL;

  Addr my_addr = Curr_Header;//give address to free mem do not include header

  //remove_header();
  return my_addr;

  // if (FREE_LIST[index] != NULL){

  // } else {

  // }

  // for (int i =0; i <NUM_LISTS; ++i){

  // }

  // int i_f, j_f;
  // for (int i = 0; i < NUM_LISTS; ++i) {
  //   for (int j = 0; j < 2; ++j) { 
  //     if (FREE_LIST[i][j] != NULL) {
  //       // get the temp header
  //       temp_header = (Header*)FREE_LIST[i][j];
        
  //       // if it fits, set touse
  //       if (temp_header->size >= _length) {
  //         to_use = (Header*)FREE_LIST[i][j];
  //         i_f = i;
  //         j_f = j;
  //         break;
  //       }
  //     }
  //   }
  // }
      
  // // if we got a size that fits
  // if (to_use != NULL) {
  //   int sizeAfterSplit;
  //   sizeAfterSplit = (to_use->size + HEADER_SIZE) / 2 - HEADER_SIZE;
    
  //   // if the very first block, or otherwise splittable [ ][x] - -fixed!
  //   if (1==1) {
  //     while (sizeAfterSplit >= _length && i_f > 0) {
  //       // update sizeAfterSplit
  //       sizeAfterSplit = (to_use->size + HEADER_SIZE) / 2 - HEADER_SIZE;
        
  //       printf("Split %i Into\t",to_use->size + HEADER_SIZE);
        
  //       // update the first header
  //       to_use->size = sizeAfterSplit;
  //       to_use->empty = 1;
        
  //       printf("[%i][%i]\n",sizeAfterSplit + HEADER_SIZE,sizeAfterSplit + HEADER_SIZE);
        
  //       // create a new header at halfway of touse
  //       Header *hNew = (Header*)(((void*)to_use) + (to_use->size));
        
  //       // set the data for the new header
  //       hNew->empty = 1;
  //       hNew->size = sizeAfterSplit;
        
  //       // point them at each other
  //       tNext = to_use->next;
  //       to_use->next = hNew;
  //       hNew->prev = to_use;
  //       hNew->next = tNext;
      
  //       // update the FREE_LIST
  //       // 1. remove the old one
  //       FREE_LIST[i_f][j_f] = NULL;
        
  //       // 2. add the new Headers
  //       i_f --;
  //       if (j_f < 1) j_f ++;
  //       FREE_LIST[i_f][0] = to_use;
  //       FREE_LIST[i_f][1] = hNew;
                          
  //       to_use = hNew;
  //     }
  //   }
    
  //   // allocate now
  //   to_use->empty  = 0;
  //   FREE_LIST[i_f][j_f] = NULL;
    
  //   print();
    
  //   return (Addr)to_use;
    
  // } else {
  //   printf("ERROR: OUT OF MEMORY!\n");
  //   exit(EXIT_FAILURE);
  // }

  //return malloc((size_t)_length);
}

extern int my_free(Addr _a) {
  /* Same here! */
  // Header *temp_header = (Header*)_a;
  // Header *o;
  // int l = getList(temp_header);
   
  // temp_header->empty = 1;
    
  // if (FREE_LIST[l][0] == NULL) {
  //   FREE_LIST[l][0] = temp_header;
  //   printf("my_free(%i) at [%i][%i]\t",temp_header->size + HEADER_SIZE,l,0);
  // } else if (FREE_LIST[l][1] == NULL) {
  //   FREE_LIST[l][1] = temp_header;
  //   printf("my_free(%i) at [%i][%i]\t",temp_header->size + HEADER_SIZE,l,1);
  // }
      
  // if (temp_header->next != NULL) {
  //   o = temp_header->next;
      
  //     // [_a][o]
  //     if (o->empty == 1 && o->size == temp_header->size) {
        
  //       temp_header->size = (temp_header->size + HEADER_SIZE)*2 - HEADER_SIZE;
  //       temp_header->next = o->next;
  //       if (o->next != NULL) o->next->prev = temp_header;
  //       temp_header->empty = 1;
        
  //       FREE_LIST[l][0] = NULL;
  //       FREE_LIST[l][1] = NULL;
        
  //       FREE_LIST[l+1][1] = temp_header;
  //       printf("coalesce with (%i) (case [_a][o]) to [%i][%i]\n",temp_header->size + HEADER_SIZE,l+1,1);
  //       my_free((Addr*)temp_header);
  //     }
  //   }
    
  //   if (temp_header->prev != NULL) {
  //     o = temp_header->prev;
      
  //     // [o][_a]
  //     if (o->empty == 1 && o->size == temp_header->size) {
        
  //       o->size = (temp_header->size + HEADER_SIZE)*2 - HEADER_SIZE;
  //       o->next = temp_header->next;
  //       if (temp_header->next != NULL) temp_header->next->prev = o;
  //       o->empty = 1;
        
  //       FREE_LIST[l][0] = NULL;
  //       FREE_LIST[l][1] = NULL;
        
  //       FREE_LIST[l+1][1] = o;
  //       printf("coalesce with (%i) (case [o][_a]) to [%i][%i]\n",temp_header->size + HEADER_SIZE,l+1,1);
  //       my_free((Addr*)o);
  //     }
  //   }   
    
  //   printf("just set to 1\n");
}

int release_allocator(){
  /* This function returns any allocated memory to the operating system.
       After this function is called, any allocation fails.
    */
  free(START_MEMORY);
  printf("Successfully Deallocated Memory\n");
  return 0;
}