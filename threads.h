//
// Created by Placinta on 5/18/15.
//

#ifndef ALGS_THREADS_H
#define ALGS_THREADS_H

#include <thread>

void testThreads() {
    std::cout << "Test mutexes.\n";

    std::mutex a;
    std::thread t1([&a](){
        for (int i = 0; i < 2; i++) {
            a.lock();
            std::cout << "Thread 1\n";
            a.unlock();
        }
    });
    std::thread t2([&a](){
        for (int i = 0; i < 2; i++) {
            a.lock();
            std::cout << "Thread 2\n";
            a.unlock();
        }
    });

    t1.join();
    t2.join();
}

#endif //ALGS_THREADS_H
