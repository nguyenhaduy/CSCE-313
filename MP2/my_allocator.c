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

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    Header** LIST_PTR;
    int LIST_SIZE;
    int BASE;
    uintptr_t MEM_BEGIN;//pointer to beginning of memory block for bit calculations

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

void check_block(){
	
}

void slipt_block(){
	
}

void remove_header (int i){

}

void merge_block(){
	
}

void check_header(){

}

/* Don't forget to implement "init_allocator" and "release_allocator"! */

extern unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length){ 
  BASE = _basic_block_size;//for simplified use in other function
  unsigned int remainder = _length % BASE;
  unsigned int total = _length - remainder; //take away memory that will not fit into multiple of basic block size
  if (remainder != 0)
      total = total + BASE; //increment to allocate an extra block for the left over bytes
  _length = total;
  LIST_SIZE = round(log2(total) - log2(BASE) +.5); //determine number of lists add .5 to always round up
  //LIST_SIZE = round(LIST_SIZE+.5);//add .5 to always round up
  Header** free_list = (Header**)malloc(2 * LIST_SIZE * sizeof(Header*)); //free list array

  free_list[0] = (Header*)malloc(total); //tell os to get continuous chunk of memory and store on free_list
  MEM_BEGIN = (uintptr_t)free_list[0];
  for (int i = 1; i<LIST_SIZE*2; ++i){
    free_list[i] = NULL; //initalize pointers to Null
  }
  Header* ptr = free_list[0]; //set pointer that will be used to create headers to beginning of list
  Header tmp_head;
  tmp_head.check = '~';
  tmp_head.next = NULL;
  // printf("%f list size for the memory\n",list_size);
  
  for (int i = 0; i<LIST_SIZE*2; i+=2) {
    tmp_head.size = (BASE * pow(2,(LIST_SIZE - ((i+2)/2))));
    //   printf("\n%d working iteration %d  power is %d\n",total, i/2,power(2,(list_size-(((i+2)/2)))));
    if (total >= (BASE * pow(2,(LIST_SIZE - (((i+2)/2)))))) {//so that the first pointer only gets initialized once
      free_list[i] = ptr; //else ptr is equal to null
	}
    while (total >= (BASE * pow(2,(LIST_SIZE - (((i+2)/2)))))) {
      ptr[0] = tmp_head;
      ptr->next = &ptr[(int)((BASE * pow(2,(LIST_SIZE - (((i+2)/2)))))/sizeof(Header))];
      ptr = ptr->next;
      total = total - BASE * pow(2,(LIST_SIZE - (((i+2)/2))));
      //     printf("total mem unallocated %d\n",total);
      if (ptr->size == free_list[i]->size)
        free_list[i+1] = ptr;
      else
        free_list[i+1] = free_list[i];
      }
  }
  LIST_PTR = free_list;

  return  _length; //if successful return amount of memory made available
}

extern Addr my_malloc(unsigned int _length) {
  /* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
  */
  
  return malloc((size_t)_length);
}

extern int my_free(Addr _a) {
  /* Same here! */
  Header* my_addr=(Header*)_a;
  int* eraser;
  eraser=(int*)my_addr;//pointer to erase memory
  //printf("freeing block! size\n");
  my_addr=&my_addr[-1]; //shift pointer from used mem to header
  //int sz=my_addr->size;
  //printf(" %d\n",sz);
  if (check_header(my_addr)==1)
    printf("my_free error block already free!\n");
  if (check_header(my_addr)==-1)
    printf("my_free error not a header!\n");
  my_addr->symbol='~';//set symbol to free
  block_join(my_addr);
  int i=0;
  while((my_addr->size-sizeof(Header))>(i*sizeof(int)))
  {
    eraser[i]&=0;
    ++i;
  }
  i=0;

  while (my_addr->size>(base*power(2,i))) //find index
    ++i;
  i=list_size-1-i;//inverse index to access list

  if (list_ptr[i*2]==NULL) //add to list
  {
    list_ptr[i*2]=my_addr;
    list_ptr[i*2+1]=my_addr;
  }
  else
  {
    my_addr->next=list_ptr[i*2];
    list_ptr[i*2]=my_addr;
  }

  free(_a);
  return 0;
}

int release_allocator(){
  /* This function returns any allocated memory to the operating system.
       After this function is called, any allocation fails.
    */
  free((Addr)LIST_PTR);
  free((Addr)MEM_BEGIN);
  printf("Successfully Deallocated Memory\n");
  return 0;
}