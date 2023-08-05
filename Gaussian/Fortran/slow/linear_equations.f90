!===========================================================================
! FILE          : linear_equations.f90
! LAST REVISION : 2015-04-03
! SUBJECT       : Example program that solves simultaneous equations.
!
! This program is a lightly modified version of a program that is shown as an example in
! "Fortran 90 Programming" by Ellis, Philips, and Lahey. Published by Addison Wesley, (C)
! Copyright 1994, ISBN=0-201-54446-6. Section 18.5, page 647.
!
! In this version of the program I changed the dummy arguments to explicit-shape arrays rather
! than assumed-shape arrays. The handling of assumed-shape arrays was less efficient (at least
! with Compaq Fortran v5.5) because of the need to handle the stride information at run time.
!
! This version follows the example in the book in that it organizes the computation in a "row-
! oriented" manner. Unfortunately this plays poorly with the memory cache since two dimensional
! arrays in Fortran are stored in column-major order. Thus scanning down a row entails hopping
! from one memory location to another distant location, causing frequent cache misses.
!
! Please send comments or bug reports to
!
!      Peter C. Chapin
!      Computer Information Systems Department
!      Vermont Technical College
!      Williston, VT 05495
!      pchapin@vtc.edu
!===========================================================================

MODULE linear_equations
  IMPLICIT NONE
  PRIVATE
  PUBLIC :: gaussian_solve

  CONTAINS

    SUBROUTINE gaussian_solve(a, b, n, error)
      ! This subroutine solves the linear system Ax = b where the coefficients of A are stored
      ! in the array a. The solution is put in the array b. The variable error indicates if an
      ! error occured.

      ! Dummy arguments
      INTEGER, INTENT(IN)                  :: n
      REAL, DIMENSION(n, n), INTENT(INOUT) :: a
      REAL, DIMENSION(n), INTENT(INOUT)    :: b
      INTEGER, INTENT(OUT)                 :: error

      ! Reduce the equations by Gaussian elimination.
      CALL gaussian_elimination(a, b, n, error)

      ! If successful, compute solution with back substitution.
      IF (error == 0) CALL back_substitution(a, b, n, error)
    END SUBROUTINE gaussian_solve

    SUBROUTINE gaussian_elimination(a, b, n, error)
      ! This subroutine performs Gaussian elimination on a system of linear equations. The dummy
      ! arguments are as described above.

      ! Dummy arguments.
      INTEGER, INTENT(IN)                  :: n
      REAL, DIMENSION(n, n), INTENT(INOUT) :: a
      REAL, DIMENSION(n), INTENT(INOUT)    :: b
      INTEGER, INTENT(OUT)                 :: error

      ! Local variables
      REAL, DIMENSION(n)    :: temp_array
      INTEGER, DIMENSION(1) :: ksave
      INTEGER               :: i, j, k
      REAL                  :: temp, m

      ! Validity checks.
      IF (n == 0) THEN
         error = -1         ! There is no problem to solve.
         RETURN
      END IF

      ! Do the deed.
      error = 0
      DO i = 1, n - 1
         ! Find row with largest value of |a(j, i)|, j=i, ..., n
         ksave = MAXLOC(ABS(a(i:n, i)))

         ! Check for largest |a(j, i)| zero
         k = ksave(1) + i - 1
         IF (ABS(a(k, i)) <= 1.0E-6) THEN
            error = -2      ! No solution possible.
            RETURN
         END IF

         ! Exchange rows i and column k if necessary.
         IF (k /= i) THEN
            temp_array = a(i, :)
            a(i, :) = a(k, :)
            a(k, :) = temp_array
            ! Exchange corresponding elements of b.
            temp = b(i)
            b(i) = b(k)
            b(k) = temp
         END IF

         ! Subtract multiples of row i from subsequent rows.
         DO j = i + 1, n
            m = a(j, i)/a(i, i)
            a(j, :) = a(j, :) - m*a(i, :)
            b(j)    = b(j)    - m*b(i)
         END DO
      END DO
    END SUBROUTINE gaussian_elimination

    SUBROUTINE back_substitution(a, b, n, error)
      ! This subroutine performs a back substitutions once a system of equations has been
      ! reduced by Gaussian elimination.

      ! Dummy arguments
      INTEGER, INTENT(IN)               :: n
      REAL, DIMENSION(n, n), INTENT(IN) :: a
      REAL, DIMENSION(n), INTENT(INOUT) :: b
      INTEGER, INTENT(OUT)              :: error

      ! Local variables
      REAL    :: sum
      INTEGER :: i, j

      error = 0

      ! Solve for each variable in turn
      DO i = n, 1, -1
         ! Check for zero coefficient
         IF (ABS(a(i, i)) <= 1.0E-6) THEN
            error = -2
            RETURN
         END IF

         sum = b(i)
         DO j = i + 1, n
            sum = sum - a(i, j)*b(j)
         END DO
         b(i) = sum/a(i, i)
      END DO
    END SUBROUTINE back_substitution
    
 END MODULE linear_equations
