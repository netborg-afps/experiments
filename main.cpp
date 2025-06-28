#include "benchmark.h"
#include "001_threading_overload/001.h"


int main(int argc, char* argv[]) {

    const char* benchName = "";
    if (argc > 1)
       benchName = argv[1];

    for( int i=0; i<10; ++i )
        _001::run(benchName);

    _001::finish();

    return 0;

}
