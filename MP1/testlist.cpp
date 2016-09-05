#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
#include <string.h>

int main(int argc, char *argv[])
{
	int c;
	int x = 0, y = 0, z = 0;
	c = getopt(argc, argv, "b:s:");
	// printf("ARG: %d\n", c);
	// printf("%d\n", argc);
	if (argc > 1) {
		printf("Flag is %c and argument is %s\n", optopt, optarg);
		printf("Size of %s is %lu\n", optarg, sizeof(optarg));
	}
	char *b = optarg;
	int M = (int)(*b) * 12;  // so we have space for 12 items
	// // their getopt() code, keep intact

	char buf [1024];
	memset (buf, 1, 1024);		// set each byte to 1
	char * msg = "a sample message";

	Init (M, *b); // initialize
	// test operations
	int testnums [] = {100, 5, 200, 7, 39, 25, 400, 50, 200, 300};
	int i = 0;
	// some sample insertions
	for (i = 0; i < 10; i ++)
	{
		Insert (testnums [i], buf, 50);   // insert 50 bytes from the buffer as value for each of the insertions
	}
	PrintList ();
	Insert (150, buf, 200); // this Insert should fail
	Delete (200);
	Delete (7);
	PrintList ();
	Insert (13, msg, strlen(msg) + 1);		// insertion of strings, copies the null byte at the end
	Delete (100);
	Delete (300);
	PrintList ();
	Insert (500, buf, 50);
	Insert (600, buf, 50);
	PrintList ();

	// a sample lookup operations that should return null, because it is looking up a non-existent number
	int _key = 3;
	char* kv = Lookup (_key);
	if (kv)
		printf ("Key = %d, Value Len = %d\n", *(int *) kv, *(int *) (kv + 4));
	else
		printf ("Key = %d could not be found\n", _key);


	// this look up  should succeed and print the string "a sample message"
	_key = 13;
	kv = Lookup (_key);
	if (kv)
		printf ("Key = %d, Value Len = %d, Value = %s\n", *(int *) kv, *(int *) (kv + 4), kv + 8);
	else
		printf ("Key = %d could not be found\n", _key);

	// end test operations
	Destroy ();

	return 0;
}