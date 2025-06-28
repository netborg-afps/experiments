#pragma once
#include <fstream>
#include <atomic>
#include <assert.h>
#include <format>
#include <iostream>

#include "time.h"


class Benchmark {
public:

    Benchmark( const char* fileName,  const char* benchName="" )
    : m_benchName(benchName) {

        m_file.open( fileName, std::ios::app );
        assert( m_file.is_open() );

    }

    /*
     * may be called by many threads to catch the start most accurately
     */
    void start() {

        time_point_t t0 = { };
        time_point_t t1 = now();

        m_start.compare_exchange_strong( t0, t1 );

    }

    /*
     * should be called by one thread only
     */
    void finish() {

        m_finish.store( now() );
        m_results.push_back( duration( m_start, m_finish) );

    }


    void reset(const char* benchName) {

        if (m_benchName != benchName) {
            m_benchName = benchName;
            m_file << std::endl << m_benchName << " ";
        }

        time_point_t t0 = { };
        m_start.store( t0 );
        m_finish.store( t0 );

    }


    std::string getSeconds( int64_t microseconds ) {

        int64_t milliseconds = microseconds / 1000;
        return std::format("{}.{}", milliseconds/1000, milliseconds%1000 );

    }


    void writeSingleResult() {

        std::string s(
            std::format( "  benchmark {} finished in {} seconds",
                m_benchName,
                getSeconds( duration(m_start, m_finish) ) )
        );

        std::cout << s << std::endl;
        m_file << getSeconds( duration(m_start, m_finish) ) << ", ";

    }


    void writeResultStats() {

        int64_t sum = 0;
        for (int64_t us : m_results) {
            sum += us;
        }

        int64_t avg = !m_results.empty() ? sum / m_results.size() : 0;

        std::cout << "  benchmark " << m_benchName << " avg = " << getSeconds(avg) << " seconds" << std::endl;
        m_file << std::endl << m_benchName << " avg: " << getSeconds(avg) << " sec";

    }

private:

    const char* m_benchName;
    std::ofstream m_file;
    std::atomic< time_point_t > m_start  = { };
    std::atomic< time_point_t > m_finish = { };

    std::vector< int64_t > m_results;

};
