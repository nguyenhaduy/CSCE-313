#ifndef _semaphore_H_
#define _semaphore_H_

#include <pthread.h>

class Semaphore {
private:
    int value;
    pthread_mutex_t m;
    pthread_cond_t c;

public:

    Semaphore(int _val){
        value = _val;
        pthread_cond_init(&c, NULL);
        pthread_mutex_init(&m, NULL);
    }

    ~Semaphore(){
        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&c);
    }

    int P(){

        int err_code;
        if (err_code = pthread_mutex_lock(&m) != 0)
            return err_code;
        while (value <=0) {
            if (err_code = pthread_cond_wait(&c, &m) != 0)
                return err_code;
        }
        --value;
        if (err_code = pthread_mutex_unlock(&m) != 0)
            return err_code;

        return 0;
    }

    int V(){

        int err_code;
        if (err_code = pthread_mutex_lock(&m) != 0)
            return err_code;
        ++value;
        if (err_code = pthread_cond_broadcast(&c) != 0)
                return err_code;


        if (err_code = pthread_mutex_unlock(&m) != 0)
            return err_code;

        return 0;
    }

};


#endif