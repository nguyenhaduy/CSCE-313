#include <cassert>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sstream>

#include "NetworkRequestChannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

struct sockaddr_in serverIn;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

const int MAX_MESSAGE = 255;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* methods */
/*--------------------------------------------------------------------------*/

// uses tcp to create a client connection to server
int createClientConnection(const char * host, const char * port) {
	struct sockaddr_in sockIn;
	memset(&sockIn, 0, sizeof(sockIn));
	sockIn.sin_family = AF_INET;
	
	// Establish port
	if (struct servent * pse = getservbyname(port, "tcp")) {
		sockIn.sin_port = pse->s_port;
	} else if ((sockIn.sin_port = htons((unsigned short)atoi(port))) == 0) { 
		cout << "Error Connecting to Port " << atoi(port);
		exit(-1);
	}
	
	// Connect to host
	if (struct hostent * hn = gethostbyname(host)) {
		memcpy(&sockIn.sin_addr, hn->h_addr, hn->h_length);
	} else if((sockIn.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		cout << "Error Conneting to Host <" << host << ">";
		exit(-1);
	}
	
	// Creating a socket
	int s = socket(AF_INET, SOCK_STREAM, 0); //SOCK_STREAM finds a socket for a TCP connection
	if (s < 0) {
		cout << "Error Creating Socket";
		exit(-1);
	}
	
	// Make the Connection
	if (connect(s, (struct sockaddr *)&sockIn, sizeof(sockIn)) < 0) {
		cout << "Error Connecting to <" << host << "> at port: " << port;
		exit(-1);
	}
	
	return s;
}

// create the server connection using passive tcp
int createServerConnection(const char * svc, int backlog) {

	memset(&serverIn, 0, sizeof(serverIn));
	serverIn.sin_family = AF_INET;
	serverIn.sin_addr.s_addr = INADDR_ANY;
	
	// Map to the Port
	if (struct servent * pse = getservbyname(svc, "tcp")) {
		serverIn.sin_port = pse->s_port;
	} else if ((serverIn.sin_port = htons((unsigned short)atoi(svc))) == 0) {
		cout << "Error Mapping to Port!!!";
		exit(-1);
	}
		
	// Bind to Port
	int snum  = socket(AF_INET, SOCK_STREAM, 0);	
	if (snum < 0) {
		cout << "Error Creating Socket!!!";
		exit(-1);
	}
	
	if (bind(snum, (struct sockaddr *)&serverIn, sizeof(serverIn)) < 0) {
		cout << "Error binding!!!";
		exit(-1);
	}
	
	// Start Listening for Request
	if (listen(snum, backlog) < 0) {
		cout << "Error Start Listening!!!";
		exit(-1);
	}
		
	return snum;
}

/*--------------------------------------------------------------------------*/
/* constructor and destructor  */
/*--------------------------------------------------------------------------*/

// Constructor for Client. Create a CLIENT_SIDE copy of the channel.
NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short _port) {
	stringstream ss;
	ss << _port;
	string port = ss.str();
	
	fd = createClientConnection(_server_host_name.c_str(), port.c_str());	
}

// Constructor for Server. Create a SERVER_SIDE copy of the channel.
NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port, void * (*connection_handler) (void *), int backlog) {
	stringstream ss;
	ss << _port;
	string port = ss.str();
	
	int master_socket = createServerConnection(port.c_str(), backlog);
	int serverSize = sizeof(serverIn);	

	while (true) {
		int * slave_socket = new int;
		
		pthread_t thread;
		pthread_attr_t attr; 
		pthread_attr_init(&attr);			
		
		*slave_socket = accept(master_socket,(struct sockaddr*)&serverIn, (socklen_t*)&serverSize);
		
		if (slave_socket < 0) {
			delete slave_socket;
			
			if (errno == EINTR) continue;
			else {
				cout << "Accept failure!";
				exit(-1);
			}
		}	
		
		pthread_create(&thread, &attr, connection_handler, (void*)slave_socket);		
 
	}
	cout << "Connection established";
}

// Destructor
NetworkRequestChannel::~NetworkRequestChannel() {
	close(fd); // Closes the socket
}

/* Blocking read of data from the channel. Returns a string of characters
     read from the channel. Returns NULL if read failed. */
string NetworkRequestChannel::cread() {
	char buf[MAX_MESSAGE];
	
	if (read(fd, buf, MAX_MESSAGE) < 0) {
		perror("Can't read");
		exit(-1);
	}	
	
	string s = buf;

	return s;
}

/* Write the data to the channel. The function returns the number of characters written
     to the channel. */
int NetworkRequestChannel::cwrite(string _msg) {
	if (_msg.length() >= MAX_MESSAGE) {
		cout << "Message exceeded MAX_MESSAGE";
		return -1;
	}
	
	const char * s = _msg.c_str();

	if (write(fd, s, strlen(s)+1) < 0) {
		perror("Can't write.");
		exit(-1);
	}
}

/* Send a string request over the channel and wait for a reply. */
string NetworkRequestChannel::send_request(string _request) {
  cwrite(_request);
  string s = cread();
  return s;
}

// gets the file descriptor
int NetworkRequestChannel::get_fd() { return fd; }