#ifndef _BoundedBuffer_H_
#define _BoundedBuffer_H_

#include <pthread.h>
#include <queue>
#include "semaphore.h"

template <typename T> class BoundedBuffer{

    int size;
    Semaphore * lock, * full, * empty;
    queue<T> buffer;


public:

	BoundedBuffer(){
	}

    BoundedBuffer(int _size) {
        size = _size;
        lock = new Semaphore(1);
        full = new Semaphore(0);
        empty = new Semaphore(size);
    }

    void push(T data){
        empty->P();
        lock->P();
        buffer.push(data);
        lock->V();
        full->V();
    }

    T pop(){
        full->P();
        lock->P();

        T output = buffer.front();
        buffer.pop();

        lock->V();
        empty->V();
        return output;
    } 
};


#endif