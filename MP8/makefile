# makefile

all: dataserver client client2

NetworkRequestChannel.o: NetworkRequestChannel.h NetworkRequestChannel.cpp
	g++ -c -g NetworkRequestChannel.cpp

dataserver: dataserver.cpp NetworkRequestChannel.o
	g++ -std=c++11 -g -o dataserver dataserver.cpp NetworkRequestChannel.o -lpthread

client: client_MP6.cpp NetworkRequestChannel.o
	g++ -std=c++11 -g -o client client_MP6.cpp NetworkRequestChannel.o -lpthread

client2: client_MP7.cpp NetworkRequestChannel.o
	g++ -std=c++11 -g -o client2 client_MP7.cpp NetworkRequestChannel.o -lpthread

clean:
	rm *.o fifo* dataserver client client2 output*