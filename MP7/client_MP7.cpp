/*
    File: client_MP6.cpp

    Author: J. Higginbotham
    Department of Computer Science
    Texas A&M University
    Date  : 2016/05/21

    Based on original code by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31

    MP6 for Dr. //Tyagi's
    Ahmed's sections of CSCE 313.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */
    /* -- This might be a good place to put the size of
        of the patient response buffers -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*
    As in MP7 no additional includes are required
    to complete the assignment, but you're welcome to use
    any that you think would help.
*/
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <assert.h>
#include <fstream>
#include <numeric>
#include <vector>
#include <queue>
#include "reqchannel.h"
#include "semaphore.h"
#include "BoundedBuffer.h"

/*
    This next file will need to be written from scratch, along with
    semaphore.h and (if you choose) their corresponding .cpp files.
 */

//#include "bounded_buffer.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/*
    All *_params structs are optional,
    but they might help.
 */

struct request_thread_params {
    std::string name;
    int n, w, b;
    pthread_t tid;
    pthread_mutex_t* request_mutex;
    BoundedBuffer<string>* request_buffer;
    vector<int> count;
    BoundedBuffer<int> response_buffer;
    RequestChannel *workerChannel;

    request_thread_params(const string _name, const int _n, const int _w, const int _b, BoundedBuffer<string> *buff, pthread_mutex_t* mut, RequestChannel *chan)
    :name(_name), n(_n), w(_w), b(_b), request_buffer(buff), request_mutex(mut){
        std::cout << "Creating PARAMS for " << _name << std::endl;
        vector<int> temp (10,0);
        count = temp;
        BoundedBuffer<int> temp2 (_b);
        response_buffer = temp2;
    }
};

struct worker_thread_params {
    int n, w;
    pthread_t tid;
    pthread_mutex_t* worker_mutex;
    BoundedBuffer<string>* request_buffer;
    vector<request_thread_params>* request_threads;
    RequestChannel *workerChannel, *controlChannel;

    worker_thread_params(const int _n, const int _w, BoundedBuffer<string> *buff, pthread_mutex_t* mut, vector<request_thread_params>* p, RequestChannel *chan)
    :n(_n), w(_w), request_buffer(buff), worker_mutex(mut), request_threads(p), controlChannel(chan){
        // std::string s = chan->send_request("newthread");
        // workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
    }
};

struct stat_thread_params {
    int n, w;
    pthread_t tid;
    pthread_mutex_t* stat_mutex;
    request_thread_params* request_thread;
    std::string results;

    stat_thread_params(const int _n, const int _w, pthread_mutex_t* mut, request_thread_params* p)
    :n(_n), w(_w), stat_mutex(mut), request_thread(p){}
};

/*
    This class can be used to write to standard output
    in a multithreaded environment. It's primary purpose
    is printing debug messages while multiple threads
    are in execution.
 */
class atomic_standard_output {
    pthread_mutex_t console_lock;
public:
    atomic_standard_output() { pthread_mutex_init(&console_lock, NULL); }
    ~atomic_standard_output() { pthread_mutex_destroy(&console_lock); }
    void print(std::string s){
        pthread_mutex_lock(&console_lock);
        std::cout << s << std::endl;
        pthread_mutex_unlock(&console_lock);
    }
};

atomic_standard_output threadsafe_standard_output;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* HELPER FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string make_histogram(std::string name, std::vector<int> *data) {
    std::string results = "Frequency count for " + name + ":\n";
    for(int i = 0; i < data->size(); ++i) {
        results += std::to_string(i * 10) + "-" + std::to_string((i * 10) + 9) + ": " + std::to_string(data->at(i)) + "\n";
    }
    return results;
}

/*
    You'll need to fill these in.
*/
void* request_thread_function(void* arg) {

    request_thread_params *patient = (request_thread_params*) arg;
    string data = "data ";
    data = data + patient->name;

    for(int i = 0; i < patient->n; ++i) {
        patient->request_buffer->push(data);
    }
}

void* worker_thread_function(void* arg) {

    worker_thread_params *worker = (worker_thread_params*)arg;
    
    vector<request_thread_params> *request_threads = worker->request_threads;
    
    while(true) {
        std::string request = worker->request_buffer->pop();
        std::string response = worker->workerChannel->send_request(request);
        if(request == "data John Smith") {
                (*request_threads)[0].response_buffer.push(stoi(response));
            }
            else if(request == "data Jane Smith") {
                (*request_threads)[1].response_buffer.push(stoi(response));
            }
            else if(request == "data Joe Smith") {
                (*request_threads)[2].response_buffer.push(stoi(response));
                
            }
            else if (request == "quit"){
                delete worker->workerChannel;
                break;
            }        
    }
}

void* event_handler_thread_function(void* arg) {

    worker_thread_params *worker = (worker_thread_params*)arg;
    
    vector<request_thread_params> *request_threads = worker->request_threads;

    RequestChannel* NetworkChannels[worker->w];
    int channelID[worker->w];
    fd_set readset;	//readfile descriptor
    int max = 0;
    int selectResult;
    int sent_reqs = 0;
    int recvd_resps = 0;
    int rcount = 0;
    struct timeval timeout = {0,10};    //waiting time
    string request;

    for(int i = 0; i < worker->w; ++i) {
        // Create new RequestChannel
    	string reply = worker->controlChannel->send_request("newthread");
        NetworkChannels[i] = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);

        // Fill all the channels with requests before trying to read from them
        request = worker->request_buffer->pop();
        sent_reqs++;
        NetworkChannels[i]->cwrite(request);

        // Categorize channelID base on request data
        if(request == "data John Smith") {
            channelID[i] = 0;
        }
        else if(request == "data Jane Smith") {
            channelID[i] = 1;
        }
        else if(request == "data Joe Smith") {
            channelID[i] = 2;                
        }
    }
    
    while(recvd_resps < 3*worker->n){
    	FD_ZERO(&readset);

        // Get the max fd
        for(int i=0; i < worker->w; ++i){
            if(NetworkChannels[i]->read_fd() > max) 
            	max = NetworkChannels[i]->read_fd();
            FD_SET(NetworkChannels[i]->read_fd(), &readset);
        }

    	selectResult = select(max + 1, &readset, NULL, NULL, &timeout);

    	if(selectResult){
            for(int i = 0; i < worker->w; ++i){

                // Read from pipeline
                if(FD_ISSET(NetworkChannels[i]->read_fd(), &readset)){
                    string serverResponse = NetworkChannels[i]->cread();
                    recvd_resps++;

                    // Processing histogram data base on server responses
                    switch(channelID[i]){
                        case 0: 
                            (*request_threads)[0].response_buffer.push(stoi(serverResponse));    
                            break;
                        case 1: 
                            (*request_threads)[1].response_buffer.push(stoi(serverResponse));   
                            break;
                        case 2: 
                            (*request_threads)[2].response_buffer.push(stoi(serverResponse));   
                            break;
                    }
                    
                    // Continue to send request to the available channel
                    if(sent_reqs < 3*worker->n){
                        request = worker->request_buffer->pop();
                        sent_reqs++;
                        NetworkChannels[i]->cwrite(request);
                        if(request == "data John Smith") {
                            channelID[i] = 0;
                        }
                        else if(request == "data Jane Smith") {
                            channelID[i] = 1;
                        }
                        else if(request == "data Joe Smith") {
                            channelID[i] = 2;                
                        }
                    }
                }
            }
        }
    }
    
    // Close request channels
    for(int i = 0; i < worker->w; ++i)
        NetworkChannels[i]->send_request("quit");

    // Close control channel
    // worker->workerChannel->send_request("quit");
}

void* stat_thread_function(void* arg) {

	stat_thread_params *stat = (stat_thread_params*)arg;

	stat->results = "Frequency count for " + stat->request_thread->name + ":\n";
  
    vector<int> count (10,0);
    for (int i = 0; i < stat->request_thread->n; ++i){
    	int response = stat->request_thread->response_buffer.pop();
    	count.at(response/10) += 1;
    }    

    for(int i = 0; i < 10; ++i) {
        stat->results += std::to_string(i * 10) + "-" + std::to_string((i * 10) + 9) + ": " + std::to_string(count[i]) + "\n";
    }

    stat->request_thread->count = count;
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 10; //default number of requests per "patient"
    int b = 50; //default size of request_buffer
    int w = 10; //default number of worker threads
    bool USE_ALTERNATE_FILE_OUTPUT = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:b:w:m:h")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'm':
                if(atoi(optarg) == 2) USE_ALTERNATE_FILE_OUTPUT = true;
                break;
            case 'h':
            default:
                std::cout << "This program can be invoked with the following flags:" << std::endl;
                std::cout << "-n [int]: number of requests per patient" << std::endl;
                std::cout << "-b [int]: size of request buffer" << std::endl;
                std::cout << "-w [int]: number of worker threads" << std::endl;
                std::cout << "-m 2: use output2.txt instead of output.txt for all file output" << std::endl;
                std::cout << "-h: print this message and quit" << std::endl;
                std::cout << "Example: ./client_solution -n 10000 -b 50 -w 120 -m 2" << std::endl;
                std::cout << "If a given flag is not used, a default value will be given" << std::endl;
                std::cout << "to its corresponding variable. If an illegal option is detected," << std::endl;
                std::cout << "behavior is the same as using the -h flag." << std::endl;
                exit(0);
        }
    }
    
    int pid = fork();
    if(pid == 0){
        struct timeval start_time;
        struct timeval finish_time;
        int64_t start_usecs;
        int64_t finish_usecs;

        gettimeofday(&start_time, NULL);

        ofstream ofs;
        if(USE_ALTERNATE_FILE_OUTPUT) ofs.open("output2.txt", ios::out | ios::app);
        else ofs.open("output.txt", ios::out | ios::app);
        
        std::cout << "n == " << n << std::endl;
        std::cout << "b == " << b << std::endl;
        std::cout << "w == " << w << std::endl;
        
        std::cout << "CLIENT STARTED:" << std::endl;
        std::cout << "Establishing control channel... " << std::flush;

        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
        std::cout << "done." << std::endl;
        
        /*
            This time you're up a creek.
            What goes in this section of the code?
            Hint: it looks a bit like what went here 
            in MP7, but only a *little* bit.
        */

        pthread_mutex_t client_mutex;
        pthread_mutex_init (&client_mutex, NULL);

        BoundedBuffer<std::string> request_buffer(b);
        vector<BoundedBuffer<int>> response_buffers;
        for (int i =0; i < 3; ++i){
        	BoundedBuffer<int> temp(n);
        	response_buffers.push_back(temp);
        }

        // Creating request params
        std::cout << "Creating request_thread_params." << std::endl;
        vector <request_thread_params> request_threads;
        request_threads.push_back(request_thread_params ("John Smith", n, w, b, &request_buffer, &client_mutex, chan));
        request_threads.push_back(request_thread_params ("Jane Smith", n, w, b, &request_buffer, &client_mutex, chan));
        request_threads.push_back(request_thread_params ("Joe Smith", n, w, b, &request_buffer, &client_mutex, chan));

        std::cout << "Finished creating request_thread_params." << std::endl;

        // Creating event handler thread
        pthread_t event_handler;
        worker_thread_params event_param (n, w, &request_buffer, &client_mutex, &request_threads, chan);


        // vector<worker_thread_params> worker_threads;

        // std::cout << "Creating worker channels... ";
        // for (int i = 0; i < w; ++i){
        //     worker_threads.push_back( worker_thread_params(n, w, &request_buffer, &client_mutex, &request_threads, chan));
        // }
        // std::cout << "done." << std::endl;

        // Creating statistic param
        vector <stat_thread_params> stat_threads;
        for( int i = 0; i < request_threads.size(); ++i){
        	stat_threads.push_back(stat_thread_params (n, w, &client_mutex, &request_threads[i]));
        }

        // Start all the threads
        std::cout << "Start all the threads!!!" << std::endl;
        
        // Creating & starting request threads
        for (int i =0; i < 3; ++i){
            if(pthread_create(&request_threads[i].tid, NULL, request_thread_function, &request_threads[i])) {
                fprintf(stderr, "Error creating request thread!!!\n");
                return 1;
            }
        }

        //Starting event handler thread
        pthread_create(&event_handler, NULL, event_handler_thread_function, &event_param);        

        // //Creating & starting worker threads
        // for (int i =0; i < w; ++i){
        //     if(pthread_create(&worker_threads[i].tid, NULL, worker_thread_function, &worker_threads[i])) {
        //         fprintf(stderr, "Error creating worker thread!!!\n");
        //         return 1;
        //     }
        // }

        // Creating & starting stat threads
        for (int i =0; i < 3; ++i){
            if(pthread_create(&stat_threads[i].tid, NULL, stat_thread_function, &stat_threads[i])) {
                fprintf(stderr, "Error creating stat thread!!!\n");
				return 1;
			}
		}

		
        // Joinning request threads
        for (int i =0; i < request_threads.size(); ++i){
            pthread_join(request_threads[i].tid, NULL);
        }

        // Joinning evnet handler threads
        pthread_join(event_handler,NULL);

        // std::cout << "Pushing quit requests... ";
        // // fflush(NULL);
        // for(int i = 0; i < w; ++i) {
        //     request_buffer.push("quit");
        // }
        // std::cout << "done." << std::endl;

        // // Joining workers threads
        // for (int i =0; i < w; ++i){
        //     pthread_join(worker_threads[i].tid, NULL);
        // }

        // Joining stat threads
		for (int i =0; i < stat_threads.size(); ++i){
            pthread_join(stat_threads[i].tid, NULL);
        }


        gettimeofday(&finish_time, NULL);
        
        std::cout << "Results for n == " << n << ", w == " << w << std::endl;
        long long total_time = (finish_time.tv_sec - start_time.tv_sec)*1000000 + finish_time.tv_usec - start_time.tv_usec;        
        std::cout << "Time to completion: " << std::to_string(total_time) << " usecs" << std::endl;

        std::cout << "John Smith total: " << accumulate(request_threads[0].count.begin(), request_threads[0].count.end(), 0) << std::endl;
        std::cout << stat_threads[0].results << std::endl;
        std::cout << "Jane Smith total: " << accumulate(request_threads[1].count.begin(), request_threads[1].count.end(), 0) << std::endl;
        std::cout << stat_threads[1].results << std::endl;
        std::cout << "Joe Smith total: " << accumulate(request_threads[2].count.begin(), request_threads[2].count.end(), 0) << std::endl;
        std::cout << stat_threads[2].results << std::endl;

        ofs.close();
        std::cout << "Sleeping..." << std::endl;
        usleep(10000);
        std::string finale = chan->send_request("quit");
        std::cout << "Finale: " << finale << std::endl;
        std::cout << "Finished!!!" << endl;
    }
    else if(pid != 0) execl("dataserver", NULL);
}
