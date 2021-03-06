/*
    File: client.cpp

	Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/01/31

 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*
    This assignment does not require any additional includes,
    though you are free to add any that you find helpful.
*/
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "reqchannel.h"
#include <pthread.h>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <assert.h>
#include <cmath>
#include <numeric>
#include <list>
#include <vector>


/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

class buffer{
    pthread_mutex_t buffer_lock;
    std::list<std::string> request_buffer;
public:
    buffer() { pthread_mutex_init(&buffer_lock, NULL); }
    void push(string data){
        pthread_mutex_lock(&buffer_lock);
        request_buffer.push_back(data);
        pthread_mutex_unlock(&buffer_lock);
    }
    string pop(){
        pthread_mutex_lock(&buffer_lock);
        std::string request = request_buffer.front();
        request_buffer.pop_front();
        pthread_mutex_unlock(&buffer_lock);
        return request;
    } 
};

struct PARAMS {
    /*
        This is a helpful struct to have,
        but you can implement the program
        some other way it you'd like.
    
        For example, it can hold pointers to
        the containers that hold the histogram information
        so that all the threads combine their data
        instead of assembling "w" different histograms,
        as well as pointers to RequestChannels and
        to synchronization objects.
     */
    string name;
    int n, w;
    pthread_t tid;
    pthread_mutex_t* param_mutex;
    buffer* request_buffer;
    vector<int> count;

    PARAMS(const string patient_name, const int _n, const int _w, buffer *buff, pthread_mutex_t* mut)
    :name(patient_name), n(_n), w(_w), request_buffer(buff), param_mutex(mut){
        std::cout << "Creating PARAMS for " << patient_name << std::endl;
        vector<int> temp (10,0);
        count = temp;
    }
};

struct WORKERS {
    /*
        This is a helpful struct to have,
        but you can implement the program
        some other way it you'd like.
    
        For example, it can hold pointers to
        the containers that hold the histogram information
        so that all the threads combine their data
        instead of assembling "w" different histograms,
        as well as pointers to RequestChannels and
        to synchronization objects.
     */
    int n, w;
    pthread_t tid;
    pthread_mutex_t* worker_mutex;
    buffer* request_buffer;
    vector<PARAMS>* param;
    RequestChannel *workerChannel;

    WORKERS(const int _n, const int _w, buffer *buff, pthread_mutex_t* mut, vector<PARAMS>* p, RequestChannel *chan)
    :n(_n), w(_w), request_buffer(buff), worker_mutex(mut), param(p){
        std::string s = chan->send_request("newthread");
        workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
    }
};

/*
    The class below allows any thread in this program to use
    the variable "atomic_standard_output" down below to write output to the
    console without it getting jumbled together with the output
    from other threads.
    For example:
    thread A {
        atomic_standard_output.print("Hello ");
    }
    thread B {
        atomic_standard_output.print("World!\n");
    }
    The output could be:
    Hello World!
    or...
    World!
    Hello 
    ...but it will NOT be something like:
    HelWorllo d!
 */

class atomic_standard_output {
    /*
         Note that this class provides an example
         of the usage of mutexes, which are a crucial
         synchronization type. You will probably not
         be able to write the worker thread function
         without using these.
     */
    pthread_mutex_t console_lock;
public:
    atomic_standard_output() { pthread_mutex_init(&console_lock, NULL); }
    void print(std::string s){
        pthread_mutex_lock(&console_lock);
        std::cout << s << std::endl;
        pthread_mutex_unlock(&console_lock);
    }
};

atomic_standard_output threadsafe_standard_output;


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

void* worker_thread_function(void* arg) {
    /*
        Fill in the loop. Make sure
        it terminates when, and not before,
        all the requests have been processed.
        
        Each thread must have its own dedicated
        RequestChannel. Make sure that if you
        construct a RequestChannel (or any object)
        using "new" that you "delete" it properly,
        and that you send a "quit" request for every
        RequestChannel you construct regardless of
        whether you used "new" for it.
     */

    WORKERS *worker = (WORKERS*)arg;
    
    vector<PARAMS> *param = worker->param;
    
    while(true) {
        std::string request = worker->request_buffer->pop();
        std::string response = worker->workerChannel->send_request(request);
        if(request == "data John Smith") {
                pthread_mutex_lock(worker->worker_mutex);
                (*param)[0].count.at(stoi(response) / 10) += 1;
                pthread_mutex_unlock(worker->worker_mutex);
            }
            else if(request == "data Jane Smith") {
                pthread_mutex_lock(worker->worker_mutex);
                (*param)[1].count.at(stoi(response) / 10) += 1;
                pthread_mutex_unlock(worker->worker_mutex);
            }
            else if(request == "data Joe Smith") {
                pthread_mutex_lock(worker->worker_mutex);
                (*param)[2].count.at(stoi(response) / 10) += 1;
                pthread_mutex_unlock(worker->worker_mutex);
            }
            else if (request == "quit"){
                delete worker->workerChannel;
                break;
            }        
    }
}

void* request_thread_function(void *para){
    fflush(NULL);   
    PARAMS *patient = (PARAMS*) para;
    string data = "data ";
    data = data + patient->name;

    for(int i = 0; i < patient->n; ++i) {
        patient->request_buffer->push(data);
    }
    fflush(NULL);
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    /*
        Note that the assignment calls for n = 10000.
        That is far too large for a single thread to accomplish quickly,
        but you will still have to test it for w = 1 once you finish
        filling in the worker thread function.
     */
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:w:h")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                //This won't do a whole lot until you fill in the worker thread function
                w = atoi(optarg);
                break;
            case 'h':
            default:
                std::cout << "This program can be invoked with the following flags:" << std::endl;
                std::cout << "-n [int]: number of requests per patient" << std::endl;
                std::cout << "-w [int]: number of worker threads" << std::endl;
                std::cout << "-h: print this message and quit" << std::endl;
                std::cout << "Example: ./client_solution -n 10000 -w 120 -v 1" << std::endl;
                std::cout << "If a given flag is not used, or given an invalid value," << std::endl;
                std::cout << "a default value will be given to the corresponding variable." << std::endl;
                std::cout << "If an illegal option is detected, behavior is the same as using the -h flag." << std::endl;
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
        
        std::cout << "n == " << n << std::endl;
        std::cout << "w == " << w << std::endl;
        
        std::cout << "CLIENT STARTED:" << std::endl;
        std::cout << "Establishing control channel... " << std::flush;
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
        std::cout << "done." << std::endl;
        
        /*
            All worker threads will use these,
            but the current implementation puts
            them on the stack in main as opposed
            to global scope. They can be moved to
            global scope, but you could also pass
            pointers to them to your thread functions.
         */
        pthread_mutex_t client_mutex;
        pthread_mutex_init (&client_mutex, NULL);
        // std::list<std::string> request_buffer;

        buffer request_buffer;
        std::cout << "Creating PARAMS." << std::endl;
        vector <PARAMS> param;
        param.push_back(PARAMS ("John Smith", n, w, &request_buffer, &client_mutex));
        param.push_back(PARAMS ("Jane Smith", n, w, &request_buffer, &client_mutex));
        param.push_back(PARAMS ("Joe Smith", n, w, &request_buffer, &client_mutex));

        std::cout << "Finished creating PARAMS." << std::endl;
        std::vector<int> john_frequency_count(10, 0);
        std::vector<int> jane_frequency_count(10, 0);
        std::vector<int> joe_frequency_count(10, 0);

        
        
        /*-------------------------------------------*/
        /* START TIMER HERE */
        /*-------------------------------------------*/
        
/*--------------------------------------------------------------------------*/
/* BEGIN "MODIFY AT YOUR OWN RISK" SECTION */
/*
    But please remember to comment out the output statements
    while gathering data for your report. They throw off the timer.
*/
/*--------------------------------------------------------------------------*/
        
        std::cout << "Populating request buffer... ";
        fflush(NULL);
        
        for (int i =0; i < 3; ++i){
            if(pthread_create(&param[i].tid, NULL, request_thread_function, &param[i])) {
                fprintf(stderr, "Error creating request thread!!!\n");
                return 1;
            }
        }

        
        // if(pthread_create(&param[1].tid, NULL, request_thread_function, &param[1])) {
        // fprintf(stderr, "Error creating thread for jane\n");
        // return 1;
        // }
        // if(pthread_create(&param[2].tid, NULL, request_thread_function, &param[2])) {
        // fprintf(stderr, "Error creating thread for joe\n");
        // return 1;
        // }
        
        for (int i =0; i < 3; ++i){
            pthread_join(param[i].tid, NULL);
        }

        // for(int i = 0; i < n; ++i) {
        //     request_buffer.push("data John Smith");
        //     request_buffer.push("data Jane Smith");
        //     request_buffer.push("data Joe Smith");
        // }

        std::cout << "done." << std::endl;
        
        std::cout << "Pushing quit requests... ";
        fflush(NULL);
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
        std::cout << "done." << std::endl;
        
/*--------------------------------------------------------------------------*/
/* END "MODIFY AT YOUR OWN RISK" SECTION    */
/*--------------------------------------------------------------------------*/
        
/*--------------------------------------------------------------------------*/
/*  BEGIN CRITICAL SECTION  */
/*
    Most of the changes you make will be
    to this section of code and to the
    worker thread function from earlier, unless
    you also decide to utilize the PARAMS struct (which is
    *highly* recommended) and then you'll need
    to fill that in as well.
 
    You will modify the program so that client.cpp
    uses w threads to process the requests, instead of
    sequentially using a single loop.
*/
/*--------------------------------------------------------------------------*/

        vector<WORKERS> worker;

        std::cout << "Creating woker chanel... ";
        for (int i = 0; i < w; ++i){
            pthread_mutex_lock(&client_mutex);
            worker.push_back( WORKERS(n, w, &request_buffer, &client_mutex, &param, chan));
            pthread_mutex_unlock(&client_mutex);
        }
        std::cout << "done." << std::endl;

        std::cout << "Creating worker thread... ";
        for (int i =0; i < w; ++i){
            if(pthread_create(&worker[i].tid, NULL, worker_thread_function, &worker[i])) {
                fprintf(stderr, "Error creating worker thread!!!\n");
                return 1;
            }
        }
        std::cout << "done." << std::endl;

        std::cout << "Waiting for server response...";
        for (int i =0; i < w; ++i){
            pthread_join(worker[i].tid, NULL);
        }
        std::cout << "done." << std::endl;

        // std::cout << "Deleting worker channels...";
        // for (int i =0; i < w; ++i){
        //     delete worker[i].workerChannel;
        // }
        // std::cout << "done." << std::endl;



        // std::string s = chan->send_request("newthread");
        // RequestChannel *workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);

        
        // while(true) {
        //     std::string request = request_buffer.pop();
        //     // request_buffer.pop_front();
        //     std::string response = workerChannel->send_request(request);
            
        //     if(request == "data John Smith") {
        //         param[0].count.at(stoi(response) / 10) += 1;
        //     }
        //     else if(request == "data Jane Smith") {
        //         param[1].count.at(stoi(response) / 10) += 1;
        //     }
        //     else if(request == "data Joe Smith") {
        //         param[2].count.at(stoi(response) / 10) += 1;
        //     }
        //     else if(request == "quit") {
        //         delete workerChannel;
        //         break;
        //     }
        // }
/*--------------------------------------------------------------------------*/
/*  END CRITICAL SECTION    */
/*--------------------------------------------------------------------------*/
        
        /*
            By the point at which you end the timer,
            all worker threads should have terminated.
            Note that the containers from earlier
            are still in scope, so threads can use them
            if they have a pointer to them.
         */
        
        /*-------------------------------------------*/
        /* END TIMER HERE   */
        /*-------------------------------------------*/
        
        /*
            You may want to eventually add file output
            to this section of the code to make it easier
            to assemble the timing data from different iterations
            of the program.
         */
        start_usecs = (start_time.tv_sec * 1e6) + start_time.tv_usec;
        finish_usecs = (finish_time.tv_sec * 1e6) + finish_time.tv_usec;
        std::cout << "Finished!" << std::endl;
        
        std::string john_results = make_histogram("John Smith", &param[0].count);
        std::string jane_results = make_histogram("Jane Smith", &param[1].count);
        std::string joe_results = make_histogram("Joe Smith", &param[2].count);

        gettimeofday(&finish_time, NULL);
        
        std::cout << "Results for n == " << n << ", w == " << w << std::endl;
        long long total_time = (finish_time.tv_sec - start_time.tv_sec)*1000000 + finish_time.tv_usec - start_time.tv_usec;
        std::cout << "Time to completion: " << std::to_string(total_time) << " usecs" << std::endl;
        std::cout << "John Smith total: " << accumulate(param[0].count.begin(), param[0].count.end(), 0) << std::endl;
        std::cout << john_results << std::endl;
        std::cout << "Jane Smith total: " << accumulate(param[1].count.begin(), param[1].count.end(), 0) << std::endl;
        std::cout << jane_results << std::endl;
        std::cout << "Joe Smith total: " << accumulate(param[2].count.begin(), param[2].count.end(), 0) << std::endl;
        std::cout << joe_results << std::endl;
        
        std::cout << "Sleeping..." << std::endl;
        usleep(10000);
        
        /*
            EVERY RequestChannel must send a "quit"
            request before program termination, and
            the destructor for RequestChannel must be
            called somehow.
         */
        std::string finale = chan->send_request("quit");
        delete chan;
        pthread_mutex_destroy(&client_mutex);
        std::cout << "Finale: " << finale << std::endl; //However, this line is optional.
    }
    else if(pid != 0) execl("dataserver", NULL);
}
