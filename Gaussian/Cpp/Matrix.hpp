/*!
    \file   Matrix.hpp
    \brief  A lightweight matrix template.
    \author (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstring>

//! A lightweight matrix template.
/*!
 *  This template is a simple wrapper around a linear array of numeric values that presents the
 *  array as an n x m matrix. The template parameter must be a POD type but can otherwise be
 *  quite general. This template does not attempt to do any actual computation on the template
 *  parameter type. The name "NumericType" is intended to be suggestive of the expected
 *  applications.
 */
template< typename NumericType >
class Matrix {
public:
    // Constructors and destructors.

    //! Construct a matrix of the indicated (row, col) size.
    /*!
     *  The matrix elements are initialized as according to a new expression.
     *
     *  \param initial_n The number of rows in the matrix. Requires initial_n >= 1.
     *  \param initial_m The number of columns in the matrix. Requires initial_m >= 1.
     */
    Matrix( std::size_t initial_n, std::size_t initial_m );
   ~Matrix( );

    //! Copies another matrix to construct this matrix.
    Matrix( const Matrix &other );

    //! Assigns another matrix to this matrix.
    /*!
     *  If an exception is thrown during the assignment the target object is left unchanged.
     */
    Matrix &operator=( const Matrix &other );

    //! Returns a reference to indicated matrix element.
    /*!
     *  The behavior is undefiend if the (row, col) coordinates are outside the bounds of the
     *  matrix. Note also that row and column numbers are zero based.
     */
    NumericType &operator( )( std::size_t row, std::size_t col );
    const NumericType &operator( )( std::size_t row, std::size_t col ) const;

    //! Returns the number of rows in the matrix.
    std::size_t row_count( ) const;

    //! Returns the number of columns in the matrix.
    std::size_t col_count( ) const;

    //! Returns a pointer to the indicated row.
    /*!
     *  The row is guarenteed to be contiguous in memory. The behavior is undefined if an
     *  attempt is made to access outside the indicated row or if the argument to this method is
     *  out of range.
     */
    NumericType *get_row( std::size_t row );
    const NumericType *get_row( std::size_t row ) const;

private:
    NumericType *elements;
    std::size_t  n;
    std::size_t  m;
};


template< typename NumericType >
Matrix<NumericType>::Matrix( std::size_t initial_n, std::size_t initial_m )
    : elements( 0 ), n( initial_n ), m( initial_m )
{
    // TODO: Throw an exception if the documented requirements are violated?
    elements = new NumericType[ n * m ];
}


template< typename NumericType >
Matrix<NumericType>::~Matrix( )
{
    delete [] elements;
}

template< typename NumericType >
Matrix<NumericType>::Matrix( const Matrix &other )
    : elements( 0 ), n( other.n ), m( other.m )
{
    elements = new NumericType[ n * m ];
    std::memcpy( elements, other.elements, n * m * sizeof( NumericType ) );

}

template< typename NumericType >
Matrix<NumericType> &Matrix<NumericType>::operator=( const Matrix &other )
{
    // Allocate first so that '*this' is left unchanged if an exception is thrown here.
    NumericType *fresh_elements = new NumericType[ n * m ];

    delete [] elements;
    elements = fresh_elements;
    n = other.n;
    m = other.m;
    std::memcpy( elements, other.elements, n * m * sizeof( NumericType ) );
    return *this;
}

template< typename NumericType >
inline NumericType &
Matrix<NumericType>::operator( )( std::size_t row, std::size_t col )
{
    return elements[ row * m + col ];
}

template< typename NumericType >
inline const NumericType &
Matrix<NumericType>::operator( )( std::size_t row, std::size_t col ) const
{
    return elements[ row * m + col ];
}

template< typename NumericType >
inline std::size_t Matrix<NumericType>::row_count( ) const
{
    return n;
}

template< typename NumericType >
inline std::size_t Matrix<NumericType>::col_count( ) const
{
    return m;
}

template< typename NumericType >
inline NumericType *Matrix<NumericType>::get_row( std::size_t row )
{
    return &elements[ row * m ];
}

template< typename NumericType >
inline const NumericType *Matrix<NumericType>::get_row( std::size_t row ) const
{
    return &elements[ row * m ];
}

#endif
