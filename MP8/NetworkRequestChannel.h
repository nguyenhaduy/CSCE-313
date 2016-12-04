#ifndef _NetworkRequestChannel_H_                   // include file only once
#define _NetworkRequestChannel_H_


#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class NetworkRequestChannel {

private:

  int fd; // file descriptor for socket

public:

  /* -- CONSTRUCTOR/DESTRUCTOR */

  // Constructor for Client. Create a CLIENT_SIDE copy of the channel.
  NetworkRequestChannel(const string _host, const unsigned short _port);

  // Constructor for Server. Create a SERVER_SIDE copy of the channel.
  NetworkRequestChannel(const unsigned short _port, void * (*connection_handler) (void *), int backlog);
 
  // Destructor
  ~NetworkRequestChannel();
  
  string send_request(string _request);
  /* Send a string request over the channel and wait for a reply. */

  string cread();
  /* Blocking read of data from the channel. Returns a string of characters
     read from the channel. Returns NULL if read failed. */
  
  int cwrite(string _msg);
  /* Write the data to the channel. The function returns the number of characters written
     to the channel. */
  
  // gets the file descriptor
  int get_fd(); 
  
};


#endif