#include "ackerman.h"

int main(int argc, char ** argv) {

	// input parameters (basic block size, memory length)
	// Parse arguments using getopt()
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
	// init_allocator(basic block size, memory length)

	ackerman_main();

	// release_allocator()
}
