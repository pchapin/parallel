!===========================================================================
! FILE          : linear_equations.f90
! LAST REVISION : 2015-04-03
! SUBJECT       : Example program that solves simultaneous equations.
!
! This program is a lightly modified version of a program that is shown as an example in
! "Fortran 90 Programming" by Ellis, Philips, and Lahey. Published by Addison Wesley, (C)
! Copyright 1994, ISBN=0-201-54446-6. Section 18.5, page 647.
!
! In this version of the program I exchanged indicies so that the columns of the array
! correspond to the rows of the mathematical problem. This allows Fortran to use the processor's
! cache more effectively. I also changed the dummy arguments to explicit-shape arrays rather
! than assumed-shape arrays. The handling of assumed-shape arrays was less efficient (at least
! with Compaq Fortran v5.5) because of the need to handle the stride information at run time.
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
      INTEGER, INTENT(IN)                     :: n
      REAL*8,  DIMENSION(n, n), INTENT(INOUT) :: a
      REAL*8,  DIMENSION(n), INTENT(INOUT)    :: b
      INTEGER, INTENT(OUT)                    :: error

      ! Reduce the equations by Gaussian elimination.
      CALL gaussian_elimination(a, b, n, error)

      ! If successful, compute solution with back substitution.
      IF (error == 0) CALL back_substitution(a, b, n, error)
    END SUBROUTINE gaussian_solve

    SUBROUTINE gaussian_elimination(a, b, n, error)
      ! This subroutine performs Gaussian elimination on a system of linear equations. The dummy
      ! arguments are as described above.

      ! Dummy arguments.
      INTEGER, INTENT(IN)                     :: n
      REAL*8,  DIMENSION(n, n), INTENT(INOUT) :: a
      REAL*8,  DIMENSION(n), INTENT(INOUT)    :: b
      INTEGER, INTENT(OUT)                    :: error

      ! Local variables
      REAL*8,  DIMENSION(n) :: temp_array
      INTEGER, DIMENSION(1) :: ksave
      INTEGER               :: i, j, k
      REAL*8                :: temp, m

      ! Validity checks.
      IF (n == 0) THEN
         error = -1         ! There is no problem to solve.
         RETURN
      END IF

      ! Do the deed.
      error = 0
      DO i = 1, n - 1
         ! Find column with largest value of |a(i, j)|, j=i, ..., n
         ksave = MAXLOC(ABS(a(i, i:n)))

         ! Check for largest |a(i, j)| zero
         k = ksave(1) + i - 1
         IF (ABS(a(i, k)) <= 1.0E-6) THEN
            error = -2      ! No solution possible.
            RETURN
         END IF

         ! Exchange column i and column k if necessary.
         IF (k /= i) THEN
            temp_array = a(:, i)
            a(:, i) = a(:, k)
            a(:, k) = temp_array
            ! Exchange corresponding elements of b.
            temp = b(i)
            b(i) = b(k)
            b(k) = temp
         END IF

         ! Subtract multiples of column i from subsequent columns.
         DO j = i + 1, n
            m = a(i, j)/a(i, i)
            a(:, j) = a(:, j) - m*a(:, i)
            b(j)    = b(j)    - m*b(i)
         END DO
      END DO
    END SUBROUTINE gaussian_elimination

    SUBROUTINE back_substitution(a, b, n, error)
      ! This subroutine performs a back substitutions once a system of equations has been
      ! reduced by Gaussian elimination.

      ! Dummy arguments
      INTEGER, INTENT(IN)                  :: n
      REAL*8,  DIMENSION(n, n), INTENT(IN) :: a
      REAL*8,  DIMENSION(n), INTENT(INOUT) :: b
      INTEGER, INTENT(OUT)                 :: error

      ! Local variables
      REAL*8  :: sum
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
            sum = sum - a(j, i)*b(j)
         END DO
         b(i) = sum/a(i, i)
      END DO
    END SUBROUTINE back_substitution
    
 END MODULE linear_equations
