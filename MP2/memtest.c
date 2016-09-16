#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "my_allocator.h"
#include "ackerman.h"

/* memtest [-b <blocksize>] [-s <memsize>]
-b <blocksize> defines the block size, in bytes. Default is 128 bytes.
-s <memsize> defines the size of the memory to be allocated, in bytes.
Default is 512kB.
*/
void show_help(){
        cout <<"usage is as follows\n"
        	 <<"memtest [-b <blocksize>] [-s <memsize>]\n"
        	 <<"-b <blocksize> defines the block size, in bytes. Default is 128 bytes.\n"
        	 <<"-s <memsize> defines the size of the memory to be allocated, in bytes.\n"
        	 <<"Default is 512kB.\n\n";
}

int main(int argc, char ** argv) {

	// input parameters (basic block size, memory length)
	// Parse arguments using getopt()
	int c;
	int b = 128;
	int M = b * 4096;  // so we have space for 512kB

	if (argc<3)
        show_help();

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
	    case '?':
            if (optopt == 'c')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt)){
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                show_help();
            }else{
                fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
                show_help();
                        }
                return 1;
	    default:
	      abort ();
	    }
	}
	
	// init_allocator(basic block size, memory length)
	init_allocator(b,m);
	ackerman_main();

	// release_allocator()
	atexit((void(*)())release_allocator);//cast into correct function type
}
