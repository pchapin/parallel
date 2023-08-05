/*! \file    QuickSort.hpp
    \brief   Implementation of both sequential and parallel QuickSort.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef QUICKSORT_HPP
#define QUICKSORT_HPP

#include <algorithm>
#include <iterator>

#include <ThreadPool.hpp>

extern spica::ThreadPool master_pool;

// This code is from the Open Watcom project.
namespace seq {

    // Used for small subsequences when doing a QuickSort.
    template< typename Bidirectional, typename Compare >
    void insertion_sort( Bidirectional first, Bidirectional last, Compare comp )
    {
        if( first == last ) return;

        Bidirectional current = first;
        ++current;
        while( current != last ) {
            typename std::iterator_traits< Bidirectional >::value_type temp = *current;
            Bidirectional p1 = current;
            Bidirectional p2 = current;
            --p2;

            while( comp( temp, *p2 ) ) {
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

    template< typename RandomAccess, typename Compare >
    RandomAccess
    med3(
        RandomAccess seq,
        typename std::iterator_traits< RandomAccess >::difference_type left,
        typename std::iterator_traits< RandomAccess >::difference_type right,
        Compare comp )
    {
        using std::swap;
        typename std::iterator_traits< RandomAccess >::difference_type middle;

        middle = (left + right) / 2;
        if( comp( seq[middle], seq[left]   ) ) swap( seq[left],   seq[middle] );
        if( comp( seq[right],  seq[left]   ) ) swap( seq[left],   seq[right]  );
        if( comp( seq[right],  seq[middle] ) ) swap( seq[middle], seq[right]  );

        swap( seq[middle], seq[right - 1] );
        return seq + (right - 1);
    }


    template< typename RandomAccess, typename Compare >
    typename std::iterator_traits< RandomAccess >::difference_type
        partition(
            RandomAccess seq,
            typename std::iterator_traits< RandomAccess >::difference_type left,
            typename std::iterator_traits< RandomAccess >::difference_type right,
            Compare comp )
    {
        using std::swap;
        typedef typename std::iterator_traits< RandomAccess >::difference_type
            difference_type;

        typename std::iterator_traits< RandomAccess >::value_type
            pivot = *med3( seq, left, right, comp );

        difference_type i = left;
        difference_type j = right - 1;
        for( ; ; ) {
            while( comp( seq[++i], pivot ) ) ;
            while( comp( pivot, seq[--j] ) ) ;
            if( i >= j ) break;
            swap( seq[i], seq[j] );
        }
        swap( seq[i], seq[right - 1] );
        return i;
    }


    template< typename RandomAccess, typename Compare >
    void quick_sort(
        RandomAccess seq,
        typename std::iterator_traits< RandomAccess >::difference_type left,
        typename std::iterator_traits< RandomAccess >::difference_type right,
        Compare comp )
    {
        // The value 64 was estimated using timing experiements with VC++ v9.
        if( right - left < 64 )
            insertion_sort( seq + left, seq + right + 1, comp );
        else {
            typename std::iterator_traits< RandomAccess >::difference_type
                i = partition( seq, left, right, comp );
            quick_sort( seq, left, i - 1, comp );
            quick_sort( seq, i + 1, right, comp );
        }
    }


    template< typename RandomAccess, typename Compare >
    void sort( RandomAccess first, RandomAccess last, Compare comp )
    {
        if( first == last ) return;
        quick_sort( first, 0, ( last - first ) - 1, comp );
    }

} // namespace seq


namespace par {

    template< typename RandomAccess, typename Compare >
    struct SortParameters {
        RandomAccess seq;
        typename std::iterator_traits< RandomAccess >::difference_type left;
        typename std::iterator_traits< RandomAccess >::difference_type right;
        Compare      comp;
        int          allowed_threads;
    };


    template< typename RandomAccess, typename Compare >
    void quick_sort( void *parameters )
    {
        spica::ThreadPool::threadid_t helper_thread_id;

        SortParameters< RandomAccess, Compare > *args =
            static_cast< SortParameters< RandomAccess, Compare > * >( parameters );

        // The value 64 was estimated using timing experiements with VC++ v9.
        if( args->right - args->left < 64 )
            seq::insertion_sort( args->seq + args->left, args->seq + args->right + 1, args->comp );
        else {
            typename std::iterator_traits< RandomAccess >::difference_type
                i = seq::partition( args->seq, args->left, args->right, args->comp );

            SortParameters< RandomAccess, Compare >
                lower = { args->seq, args->left, i - 1, args->comp, 1 };
            SortParameters< RandomAccess, Compare >
                upper = { args->seq, i + 1, args->right, args->comp, 1 };

            if( args->allowed_threads == 1 ) {
                quick_sort< RandomAccess, Compare >( &lower );
                quick_sort< RandomAccess, Compare >( &upper );
            }
            else {
                lower.allowed_threads = args->allowed_threads / 2;
                upper.allowed_threads = args->allowed_threads / 2 + ((args->allowed_threads % 2 == 0) ? 0 : 1);
                helper_thread_id = master_pool.start_work(quick_sort< RandomAccess, Compare >, &upper );
                quick_sort< RandomAccess, Compare >( &lower);
                master_pool.work_result( helper_thread_id );
            }
        }
    }


    template< typename RandomAccess, typename Compare >
    void sort( RandomAccess first, RandomAccess last, Compare comp )
    {
        if( first == last ) return;
        SortParameters< RandomAccess, Compare > complete =
            { first, 0, ( last - first ) - 1, comp, master_pool.count( ) };

        quick_sort< RandomAccess, Compare >( &complete );
    }

} // namespace par

#endif
