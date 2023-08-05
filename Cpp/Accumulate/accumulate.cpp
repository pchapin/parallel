#include <algorithm>
#include <vector>

// This is sample code from the book "C++ Concurrency in Action," second edition, by Anthony
// Williams. I have reformatted the code somewhat and added comments (Peter Chapin).

// A function object that accumulates over a subsequence of a sequence and returns the result.
template<typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T &result)
    {
        result = std::accumulate(first, last, result);
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T initial)
{
    // Compute the length of the overall sequence, perhaps in O(n) time (but not necessarily).
    const unsigned long length = std::distance( first, last );
    if( length == 0 ) return initial;

    // Ensure that no thread works on an excessively small subsequence. What defines
    // "excessively" small will depend on the cost of accumulating that subsequence. If a type
    // is very simple to accumulate, the minimum number of objects in a subsequence will need to
    // be large in order to swamp thread management overheads. Perhaps this should be a template
    // parameter.
    const unsigned long min_per_thread = 1000;

    // Compute the maximum number of threads required if each thread worked on the minimum
    // subsequence.
    const unsigned long max_threads = ( length + min_per_thread - 1 ) / min_per_thread;

    // Compute the number of threads we will use. If the sequence is too small we might not want
    // to apply all available hardware threads (hence the need for max_threads). If the level of
    // hardware concurrency is not known, default to some "reasonable" value (here 2).
    const unsigned long hardware_threads = std::thread::hardware_concurrency( );
    const unsigned long num_threads =
        std::min( hardware_threads != 0 ? hardware_threads : 2, max_threads );

    // Compute the block size for all equal sized blocks. The last block might be an unusual
    // size (short) if the sequence overall is not an integer multiple of num_threads. This will
    // be handled as a special case below.
    const unsigned long block_size = length / num_threads;

    // Allocate storage for partial results and std::thread objects. Note that the thread
    // running main will take care of the last block so the vector holding std::thread objects
    // has a size one less than the overall number of threads.
    std::vector<T> results( num_threads );
    std::vector<std::thread> threads( num_threads - 1 );

    // Launch a thread to handle the first num_threads - 1 blocks.
    Iterator block_start = first;
    for( unsigned long i = 0; i < num_threads - 1; ++i ) {
        Iterator block_end = block_start;
        std::advance( block_end, block_size );  // Possibly O(n)
        threads[i] =
            std::thread( accumulate_block<Iterator, T>( ), block_start, block_end, std::ref( results[i] ) );
        block_start = block_end;
    }

    // The main thread takes care of the last block, which might (or might not) be short.
    accumulate_block<Iterator, T>( block_start, last, results[num_threads - 1] );

    // Wait for all threads to finish processing.
    for( auto &entry : threads ) entry.join( );

    // Accumulate the partial results and return the overall result.
    return std::accumulate( results.begin( ), results.end( ), initial );
}
