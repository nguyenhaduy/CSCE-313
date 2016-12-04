
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "NetworkRequestChannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

const int MAX_MESSAGE = 255;

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void handle_process_loop(int fd);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

string server_read(int * fd) {
  char buf[MAX_MESSAGE];
  

  if (read(*fd, buf, MAX_MESSAGE) < 0) {
    cout << "Error reading from client!!!";
    exit(-1);
  }
  
  string s = buf;
  return s;
}

int server_write(int * fd, string message){
  if (message.length() >= MAX_MESSAGE) {
    cout << "Message is longer than maximum characters allowed!!!";
    exit(-1);
  }
  
  if (write(*fd, message.c_str(), message.length()+1) < 0) {
    cout << "Error writing message!!!";
    exit(-1);
  }
  
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(int * fd, const string & _request)  {
  server_write(fd, "hello");
}

void process_data(int * fd, const string &  _request)  {
  usleep(1000 + (rand() % 5000)); 
  server_write(fd, int2string(rand() % 100));
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(int * fd, const string & _request) 
{
  if (_request.compare(0, 5, "hello") == 0) 
    process_hello(fd, _request);
  
  else if (_request.compare(0, 4, "data") == 0) 
    process_data(fd, _request);

  else 
    server_write(fd, "unknown request");

}

void * connection_handler(void * arg) 
{ 
  int * fd = (int*)arg;
  
  if (fd == NULL)
    cout << "No file descriptor!" << endl;
    
  while(true)  {    
    string request = server_read(fd);

    // This is for server debug
    // cout << "New request from" << request << endl;

    if (request.compare("quit") == 0) 
    {
      server_write(fd, "bye");
      usleep(10000);          // allow client more time to finish its process
      break;                  
    }

    process_request(fd, request);
  }
  
  close(*fd);  
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

  unsigned short PORT = 11987; 
  int BACKLOG_BUF = 200;  
  
  int c; 
  unsigned short _port = PORT;
  int _backlog = BACKLOG_BUF;
  
  while ((c = getopt(argc, argv, "p:b:")) != -1) {
    switch(c){
      case 'p':
        _port = atoi(optarg);
        break;
      case 'b':
        _backlog = atoi(optarg);
        break;
      default:
        std::cout << "This program can be invoked with the following flags:" << std::endl;
        std::cout << "-p [unsigned short]: port number for data server" << std::endl;
        std::cout << "-b [int]: backlog of server host" << std::endl;
        exit(-1);
        break;
    }
  }
  
  cout << "port: " << _port << "\t buffer: " << _backlog << endl; 

  NetworkRequestChannel server (_port, connection_handler, _backlog);

  cout << "Server closing..." << endl;
  server.~NetworkRequestChannel();
}

