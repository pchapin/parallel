/*! \file    matrix.hpp
    \brief   Declarations of matrix manipulation functions.
    \author  Peter C. Chapin <pcc482719@gmail.com>

*/

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cmath>
#include <cstring>
#include <stdexcept>

// The following is used by the recursive multiplication function. Should this be nested?
template< typename element_type >
struct SubMatrix {
    element_type *overall_elements;
    int overall_column_count;
    int starting_row;
    int starting_column;
    int row_count;
    int column_count;
};


// The type element_type is assumed to be a POD type.
template< typename element_type >
class Matrix {
public:
    // Exception types.
    class IncompatibleDimensions : public std::runtime_error {
    public:
        IncompatibleDimensions( )
            : std::runtime_error( "Matrix dimensions not compatible" ) { }
    };

    // Constructors/Destructors.
    Matrix( int rows, int columns );
   ~Matrix( );
    Matrix( const Matrix &other );
    Matrix &operator=( const Matrix &other );

    // Access Methods.
    int rows( ) const
        { return row_count; }

    int columns( ) const
        { return column_count; }

    element_type element( int row, int column ) const
        { return elements[ row * column_count + column ]; }

    void set_element( int row, int column, element_type value )
        { elements[ row * column_count + column ] = value; }

    // Return a SubMatrix representing the entire matrix.
    SubMatrix<element_type> overall_submatrix( ) const;

private:
    int row_count;
    int column_count;

    element_type *elements;
};


// Method Definitions
// ==================

template< typename element_type >
Matrix<element_type>::Matrix( int rows, int columns )
    : row_count( rows ), column_count( columns )
{
    elements = new element_type[ row_count * column_count ];
}

template< typename element_type >
Matrix<element_type>::~Matrix( )
{
    delete [] elements;
}

template< typename element_type >
Matrix<element_type>::Matrix( const Matrix &other )
    : row_count( other.row_count ), column_count( other.column_count )
{
    elements = new element_type[ row_count * column_count];
    std::memcpy( elements, other.elements, row_count * column_count * sizeof( element_type ) );
}


template< typename element_type >
Matrix<element_type> &Matrix<element_type>::operator=( const Matrix &other )
{
    // If the two matrices are the same size, just copy the memory. (This is a common case)
    if( row_count == other.row_count && column_count == other.column_count ) {
        std::memcpy( elements, other.elements, row_count * column_count * sizeof( element_type ) );
    }

    // Otherwise the sizes are different.
    else {
        element_type *temp = new element_type[ other.row_count * other.column_count ];
        delete [] elements;
        elements     = temp;
        row_count    = other.row_count;
        column_count = other.column_count;
        std::memcpy( elements, other.elements, row_count * column_count * sizeof( element_type ) );
    }
    return *this;
}


template< typename element_type >
SubMatrix<element_type> Matrix<element_type>::overall_submatrix( ) const
{
    SubMatrix<element_type> result;
    result.overall_elements     = elements;
    result.overall_column_count = column_count;
    result.starting_row         = 0;
    result.starting_column      = 0;
    result.row_count            = row_count;
    result.column_count         = column_count;
    return result;
}


// Relational Operators
// ====================

template< typename element_type >
bool operator==( const Matrix<element_type> &left, const Matrix<element_type> &right )
{
    // Defensive programming.
    if( (left.rows( ) != right.rows( )) || (left.columns( ) != right.columns( )) ) {
        throw typename Matrix<element_type>::IncompatibleDimensions( );
    }

    for( int i = 0; i < left.rows( ); ++i ) {
        for( int j = 0; j < left.columns( ); ++j ) {
            element_type difference = left.element( i, j ) - right.element( i, j );
            if( std::fabs( difference / left.element( i, j ) ) > 1.0E-4 ) return false;
        }
    }
    return true;
}


// Multiplication Operators
// =========================

// Computing offsets into a submatrix is a little tricky.
template< typename element_type >
inline element_type &subelement( SubMatrix<element_type> &submatrix, int row, int column )
{
    element_type *offset = submatrix.overall_elements +
        (submatrix.starting_row * submatrix.overall_column_count) + submatrix.starting_column;
    return offset[row * submatrix.overall_column_count + column];
}

// I need a version of this function that can work with constant SubMatrix objects.
template< typename element_type >
inline const element_type &subelement( const SubMatrix<element_type> &submatrix, int row, int column )
{
    element_type *offset = submatrix.overall_elements +
        (submatrix.starting_row * submatrix.overall_column_count) + submatrix.starting_column;
    return offset[row * submatrix.overall_column_count + column];
}


template< typename element_type >
void base_add(
          SubMatrix<element_type>  result,
    const SubMatrix<element_type> &left,
    const SubMatrix<element_type> &right )
{
    // Defensive programming.
    if( (left.row_count != right.row_count) || (left.column_count != right.column_count) ) {
        throw typename Matrix<element_type>::IncompatibleDimensions( );
    }

    for( int i = 0; i < result.row_count; ++i ) {
        for( int j = 0; j < result.column_count; ++j ) {
            subelement( result, i, j ) = subelement( left, i, j ) + subelement( right, i, j );
        }
    }
}


template< typename element_type >
void base_multiply(
          SubMatrix<element_type>  result,
    const SubMatrix<element_type> &left,
    const SubMatrix<element_type> &right )
{
    // Defensive programming.
    if( left.column_count != right.row_count ) {
        throw typename Matrix<element_type>::IncompatibleDimensions( );
    }

    for( int i = 0; i < result.row_count; ++i ) {
        for( int j = 0; j < result.column_count; ++j ) {
            element_type temp( 0 );
            for( int k = 0; k < left.column_count; ++k ) {
                temp += subelement( left, i, k ) * subelement( right, k, j );
            }
            subelement( result, i, j ) = temp;
        }
    }
}


template< typename element_type >
void partition_submatrix( const SubMatrix<element_type> &whole, SubMatrix<element_type> *subs )
{
    for( int i = 0; i < 4; ++i ) {
        // Common to all components.
        subs[i].overall_elements     = whole.overall_elements;
        subs[i].overall_column_count = whole.overall_column_count;

        // Deal with row information.
        if( i == 0 || i == 1 ) {
            subs[i].starting_row = whole.starting_row;
            subs[i].row_count    = whole.row_count / 2;
        }
        if( i == 2 || i == 3 ) {
            subs[i].starting_row = whole.starting_row + whole.row_count / 2;
            subs[i].row_count    = whole.row_count / 2;
            if( whole.row_count % 2 != 0 ) subs[i].row_count++;
        }

        // Deal with column information.
        if( i == 0 || i == 2 ) {
            subs[i].starting_column = whole.starting_column;
            subs[i].column_count    = whole.column_count / 2;
        }
        if( i == 1 || i == 3 ) {
            subs[i].starting_column = whole.starting_column + whole.column_count / 2;
            subs[i].column_count    = whole.column_count / 2;
            if( whole.column_count % 2 != 0 ) subs[i].column_count++;
        }
    }
}


// Recursive function that multiplies matrices.
template< typename element_type >
void multiply_helper(
          SubMatrix<element_type>  result,
    const SubMatrix<element_type> &left,
    const SubMatrix<element_type> &right )
{
    // For "small" matrix multiplications, just call the base implementation.
    if( result.row_count < 100 || result.column_count < 100 ) {
        base_multiply( result, left, right );
    }
    else {
        // Otherwise divide the problem.
        SubMatrix<element_type> subresult[4];
        SubMatrix<element_type> subleft  [4];
        SubMatrix<element_type> subright [4];

        partition_submatrix( result, subresult );
        partition_submatrix( left,   subleft   );
        partition_submatrix( right,  subright  );

        // I need some temporary matrix objects to hold intermediate computations. This can be
        // streamlined. However it is necessary for each quadrant of the result to be handled
        // by (potentially) slightly different sized matrices. This is because the partitioning
        // might not be exactly equal. Also I need two temporaries for each quadrant.
        //
        Matrix<element_type> TemporaryUL1( result.row_count / 2, result.column_count / 2 );
        Matrix<element_type> TemporaryUL2( result.row_count / 2, result.column_count / 2 );
        Matrix<element_type> TemporaryUR1( result.row_count / 2, result.column_count / 2 + ((result.column_count % 2 == 0) ? 0 : 1));
        Matrix<element_type> TemporaryUR2( result.row_count / 2, result.column_count / 2 + ((result.column_count % 2 == 0) ? 0 : 1));
        Matrix<element_type> TemporaryLL1( result.row_count / 2 + ((result.row_count % 2 == 0) ? 0 : 1), result.column_count / 2);
        Matrix<element_type> TemporaryLL2( result.row_count / 2 + ((result.row_count % 2 == 0) ? 0 : 1), result.column_count / 2);
        Matrix<element_type> TemporaryLR1( result.row_count / 2 + ((result.row_count % 2 == 0) ? 0 : 1), result.column_count / 2 + ((result.column_count % 2 == 0) ? 0 : 1));
        Matrix<element_type> TemporaryLR2( result.row_count / 2 + ((result.row_count % 2 == 0) ? 0 : 1), result.column_count / 2 + ((result.column_count % 2 == 0) ? 0 : 1));

        // Do the actual multiplication work. There are 8 subproblems.
        multiply_helper( TemporaryUL1.overall_submatrix( ), subleft[0], subright[0] );
        multiply_helper( TemporaryUL2.overall_submatrix( ), subleft[1], subright[2] );
        multiply_helper( TemporaryUR1.overall_submatrix( ), subleft[0], subright[1] );
        multiply_helper( TemporaryUR2.overall_submatrix( ), subleft[1], subright[3] );
        multiply_helper( TemporaryLL1.overall_submatrix( ), subleft[2], subright[0] );
        multiply_helper( TemporaryLL2.overall_submatrix( ), subleft[3], subright[2] );
        multiply_helper( TemporaryLR1.overall_submatrix( ), subleft[2], subright[1] );
        multiply_helper( TemporaryLR2.overall_submatrix( ), subleft[3], subright[3] );

        // Compute the final result.
        base_add( subresult[0], TemporaryUL1.overall_submatrix( ), TemporaryUL2.overall_submatrix( ) );
        base_add( subresult[1], TemporaryUR1.overall_submatrix( ), TemporaryUR2.overall_submatrix( ) );
        base_add( subresult[2], TemporaryLL1.overall_submatrix( ), TemporaryLL2.overall_submatrix( ) );
        base_add( subresult[3], TemporaryLR1.overall_submatrix( ), TemporaryLR2.overall_submatrix( ) );
    }
}


template< typename element_type >
Matrix<element_type> operator*( const Matrix<element_type> &left, const Matrix<element_type> &right )
{
    if( left.columns( ) != right.rows( ) ) {
        throw typename Matrix<element_type>::IncompatibleDimensions( );
    }
    Matrix<element_type> result( left.rows( ), right.columns( ) );
    SubMatrix<element_type> overall_left  = left.overall_submatrix( );
    SubMatrix<element_type> overall_right = right.overall_submatrix( );
    multiply_helper( result.overall_submatrix( ), overall_left, overall_right );
    return result;
}


template< typename element_type >
Matrix<element_type> OpenMPMultiply(
    const Matrix<element_type> &left, const Matrix<element_type> &right )
{
    if( left.columns( ) != right.rows( ) ) {
        throw typename Matrix<element_type>::IncompatibleDimensions( );
    }
    Matrix<element_type> result( left.rows( ), right.columns( ) );

    #pragma omp parallel for
    for( int i = 0; i < result.rows( ); ++i ) {
        for( int j = 0; j < result.columns( ); ++j ) {
            element_type temp( 0 );
            for( int k = 0; k < left.columns( ); ++k ) {
                temp += left.element( i, k ) * right.element( k, j );
            }
            result.set_element( i, j, temp );
        }
    }
    return result;
}

#endif
