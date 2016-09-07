#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "linked_list.h"

int main(int argc, char ** argv)
{
  int c;
  int b = 128;
  int M = b * 4096;  // so we have space for 512kB
  int x = 0, y = 0, z = 0;
  while ((c = getopt(argc, argv, "b:s:")) != -1 ) {
    switch (c) {
      case 'b': {
        std::stringstream ss1(optarg);
        int i;
        if ( ss1 >> i )
          b = i;
        else
          std::cout << "error";
      }
      break;
      case 's': {
        std::stringstream ss2(optarg);
        int i;
        if ( ss2 >> i ) {
          M = i;
          std::cout << M << std::endl;
        }
        else
          std::cout << "error";
      }
      break;
      default:
        break;
      }
  }

  printf("b is %d\n", b);
  printf("M is %d\n", M);
  char buf [1024];
  memset (buf, 1, 1024);    // set each byte to 1

  char * msg = "a sample message";

  Init (M, b); // initialize
  // test operations
  int testnums [] = {100, 5, 200, 7, 39, 25, 400, 50, 200, 300};
  int i = 0;
  // some sample insertions
  for (i = 0; i < 10; i ++)
  {
    Insert (testnums [i], buf, 50);   // insert 50 bytes from the buffer as value for each of the insertions
  }
  std::cout << "Inserting "
  Insert (150, buf, 200); // this Insert should fail
  PrintList ();
  Delete (7);
  Insert (13, msg, strlen(msg) + 1);    // insertion of strings, copies the null byte at the end
  Delete (55);
  Insert (15, "test msg", 8);
  Delete (3);
  PrintList ();

  // a sample lookup operations that should return null, because it is looking up a non-existent number
  char* kv = Lookup (3);
  if (kv)
    printf ("Key = %d, Value Len = %d\n", *(int *) kv, *(int *) (kv + 4));

  // this look up  should succeed and print the string "a sample message"
  kv = Lookup (13);
  if (kv)
    printf ("Key = %d, Value Len = %d, Value = %s\n", *(int *) kv, *(int *) (kv + 4), kv + 8);


  // end test operations
  Destroy ();

}
