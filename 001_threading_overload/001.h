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

    static Benchmark benchmark("001_results.txt");
    static constexpr int NUM_THREADS = 128;
    static constexpr int64_t NUM_DATA = 1'000'000;

    static std::array< int32_t, NUM_DATA > data;
    static std::atomic< int32_t > numThreadsRunning = { 0 };
    static std::atomic< int64_t > curIndex = { 0 };

    static std::unique_ptr<std::latch> allThreadsStarted;
    static std::unique_ptr<std::latch> allThreadsFinished;


    void threadLoop( int threadId ) {

        numThreadsRunning++;
        allThreadsStarted->count_down();

        // commented out because it doesn't seem to start all threads instantly
        //      allThreadsStarted->arrive_and_wait();

        while (numThreadsRunning != NUM_THREADS)
            { }

        benchmark.start();

        int64_t index;
        while (index = curIndex.load(), index<NUM_DATA) {
            if (data[index] == threadId)
                ++curIndex;
            else std::this_thread::yield();
        }

        numThreadsRunning--;
        allThreadsFinished->count_down();

    }


    void prepareData() {

        for( int32_t i=0; i<NUM_DATA; ++i )
            data[i] = std::rand() % NUM_THREADS;

    }


    void run( const char* benchName ) {

        if (curIndex == 0) {
            std::srand(1);
            std::cout << "threading overload benchmark started" << std::endl;
            std::cout << "  using " << NUM_THREADS << " threads with "
                << NUM_DATA << " data elements " << std::endl;
        }

        allThreadsStarted = std::make_unique<std::latch>(NUM_THREADS);
        allThreadsFinished = std::make_unique<std::latch>(NUM_THREADS);

        numThreadsRunning.store( 0 );
        curIndex.store( 0 );

        benchmark.reset(benchName);
        prepareData();

        std::thread** threads = new std::thread*[NUM_THREADS];
        for (int i=0; i<NUM_THREADS; ++i) {
            threads[i] = new std::thread( &threadLoop, i );
        }

        allThreadsStarted->wait();
        benchmark.start();

        allThreadsFinished->wait();
        benchmark.finish();
        benchmark.writeSingleResult();

        for (int i=0; i<NUM_THREADS; ++i) {
            threads[i]->join();
            delete threads[i];
        }

        delete[] threads;

    }


    void finish() {

        benchmark.writeResultStats();

    }

}
