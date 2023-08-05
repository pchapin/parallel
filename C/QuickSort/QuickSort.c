/*! \file    QuickSort.c
 *  \brief   Implementation of both sequential and parallel QuickSort.
 *  \author  Peter C. Chapin <pchapin@vtc.edu>
 */

#include <stdlib.h>
#include <pthread.h>
#ifdef __GLIBC__
#include <sys/sysinfo.h>
#endif

// =============
// Standard Sort
// =============

static int int_compare( const void *elem1, const void *elem2 )
{
    const int *e1 = (const int *)elem1;
    const int *e2 = (const int *)elem2;
    return *e1 - *e2;  // It's cute, but it has problems if there is integer overflow.
}

void std_sort( int *first, int *last )
{
    qsort( first, last - first, sizeof(int), int_compare );
}

// ===============
// Sequential Sort
// ===============

// Code is adapted from the Open Watcom project...

static void swap( int *x, int *y )
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

// Used for small subsequences when doing a QuickSort.
static void insertion_sort( int *first, int *last )
{
    if( first == last ) return;

    int *current = first;
    ++current;
    while( current != last ) {
        int temp = *current;
        int *p1 = current;
        int *p2 = current;
        --p2;

        while( temp < *p2 ) {
            *p1 = *p2;
            if( p2 == first ) {
                --p1;
                break;
            }
            --p1; --p2;
        }
        *p1 = temp;
        ++current;
    }
}


// Based on the QuickSort algorithm in Mark Allen Weiss's "Data Structures and Algorithm
// Analysis in C++" third edition; Addison Wesley; ISBN=0-321-44146-X.

static int *med3(int *seq, int left, int right)
{
    int middle;

    middle = (left + right) / 2;
    if( seq[middle] < seq[left]   ) swap( &seq[left],   &seq[middle] );
    if( seq[right]  < seq[left]   ) swap( &seq[left],   &seq[right]  );
    if( seq[right]  < seq[middle] ) swap( &seq[middle], &seq[right]  );
    
    swap( &seq[middle], &seq[right - 1] );
    return seq + (right - 1);
}


static int partition(int *seq, int left, int right )
{
    int pivot = *med3( seq, left, right );

    int i = left;
    int j = right - 1;
    for( ; ; ) {
        while( seq[++i] < pivot ) ;
        while( pivot < seq[--j] ) ;
        if( i >= j ) break;
        swap( &seq[i], &seq[j] );
    }
    swap( &seq[i], &seq[right - 1] );
    return i;
}


static void quick_sort( int *seq, int left, int right )
{
    // The value 64 was estimated using timing experiments with VC++ v9.
    if( right - left < 64 )
        insertion_sort( seq + left, seq + right + 1 );
    else {
        int i = partition( seq, left, right );
        quick_sort( seq, left, i - 1 );
        quick_sort( seq, i + 1, right );
    }
}


void seq_sort( int *first, int *last )
{
    if( first == last ) return;
    quick_sort( first, 0, ( last - first ) - 1 );
}

// =============
// Parallel Sort
// =============

// Uses some of the same code as seq_sort...

struct SortInterval {
    int *seq;
    int  left;
    int  right;
    int  thread_count;
};

static void *parallel_quick_sort( void *args )
{
    struct SortInterval *interval = (struct SortInterval *)args;
    struct SortInterval part_1;
    struct SortInterval part_2;

    // The value 64 was estimated using timing experiments with VC++ v9.
    if( interval->right - interval->left < 64 )
        insertion_sort( interval->seq + interval->left, interval->seq + interval->right + 1 );
    else {
        pthread_t helper_thread;

        // Split problem into sub-problems.
        int i = partition( interval->seq, interval->left, interval->right );

        // Solve the sub-problems in parallel.
        part_1.seq   = interval->seq;
        part_1.left  = interval->left;
        part_1.right = i - 1;
        part_1.thread_count = interval->thread_count/2;
        if( interval->thread_count > 1 ) {
            pthread_create( &helper_thread, NULL, parallel_quick_sort, &part_1 );
        }
        else {
            parallel_quick_sort( &part_1 );
        }

        part_2.seq   = interval->seq;
        part_2.left  = i + 1;
        part_2.right = interval->right;
        part_2.thread_count = interval->thread_count - interval->thread_count/2;
        parallel_quick_sort( &part_2 );

        if( interval->thread_count > 1 ) {
            pthread_join( helper_thread, NULL );
        }

        // Combine sub-solutions.
        // (nothing to do for QuickSort)
    }
    return NULL;
}


void par_sort( int *first, int *last )
{
    struct SortInterval complete;

    if( first == last ) return;
    complete.seq   = first;
    complete.left  = 0;
    complete.right = ( last - first ) - 1;
    #ifdef __GLIBC__
    complete.thread_count = get_nprocs( );
    #else
    complete.thread_count = pthread_num_processors_np( );
    #endif
    parallel_quick_sort( &complete );
}
