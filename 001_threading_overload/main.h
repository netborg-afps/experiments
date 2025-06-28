#pragma once

#include <thread>
#include <array>
#include <cstdlib>
#include <stdint.h>
#include <atomic>
#include <iostream>
#include <latch>


/*
 * How quickly can the kernel and scheduler handle this?
 */


namespace _001 {

    static constexpr int NUM_THREADS = 128;
    static constexpr int64_t NUM_DATA = 1'000'000;

    static std::array< uint32_t, NUM_DATA > data;
    static std::atomic< int32_t > numThreadsRunning = { 0 };
    static std::atomic< int64_t > curIndex = { 0 };

    static std::latch allThreadsStarted (NUM_THREADS);
    static std::latch allThreadsFinished (NUM_THREADS);

    void threadLoop( int threadId ) {

        numThreadsRunning++;
        allThreadsStarted.count_down();

        // commented out because it doesn't seem to start all threads instantly
        //      allThreadsStarted.arrive_and_wait();

        while( numThreadsRunning != NUM_THREADS )
            { }

        int64_t index;
        while (index = curIndex.load(), index<NUM_DATA) {
            if (data[index] == threadId)
                ++curIndex;
            else std::this_thread::yield();
        }

        numThreadsRunning--;
        allThreadsFinished.count_down();

    }


    void prepareData() {

        std::srand(1);
        for( int32_t i=0; i<NUM_DATA; ++i )
            data[i] = std::rand() % NUM_THREADS;

    }


    int main() {

        std::cout << "threading overload benchmark started" << std::endl;
        std::cout << "  using " << NUM_THREADS << " threads with "
            << NUM_DATA << " data elements " << std::endl;

        prepareData();
        std::thread** threads = new std::thread*[NUM_THREADS];
        for (int i=0; i<NUM_THREADS; ++i) {
            threads[i] = new std::thread( &threadLoop, i );
        }

        allThreadsStarted.wait();

        // start benchmark
        time_point_t start = now();

        allThreadsFinished.wait();

        time_point_t end = now();
        int64_t milliseconds = duration( start, end ) / 1000;

        std::cout << "benchmark finished in " << milliseconds << " ms " << std::endl;

        for (int i=0; i<NUM_THREADS; ++i) {
            threads[i]->join();
            delete threads[i];
        }

        delete[] threads;

        return 0;

    }

}
