!===========================================================================
! FILE          : solve_system.f90
! LAST REVISION : 2015-04-06
! SUBJECT       : Example program that solves simultaneous equations.
!
! This program is a lightly modified version of a program that is shown as an example in
! "Fortran 90 Programming" by Ellis, Philips, and Lahey. Published by Addison Wesley, (C)
! Copyright 1994, ISBN=0-201-54446-6. Section 18.5, page 647.
!
! Please send comments or bug reports to
!
!      Peter C. Chapin
!      Computer Information Systems Department
!      Vermont Technical College
!      Williston, VT 05495
!      pchapin@vtc.edu
!===========================================================================

PROGRAM solve_system
  USE linear_equations
  IMPLICIT NONE

  ! Allocatable arrays for coefficients
  REAL*8, ALLOCATABLE, DIMENSION(:, :) :: a
  REAL*8, ALLOCATABLE, DIMENSION(:)    :: b

  ! Size of arrays
  INTEGER :: n

  ! Loop variables and error flag
  INTEGER :: i, j, error

  ! Timing information
  REAL :: start_time, stop_time

  ! Get size of problem
  READ *, n

  ! Allocate arrays
  ALLOCATE(a(n, n), b(n))

  ! Get coefficients
  DO i = 1, n
     READ *, (a(j, i), j = 1, n), b(i)
  END DO

  PRINT *, "Coefficients read"

  ! Solve the system
  CALL CPU_TIME(start_time)
  CALL gaussian_solve(a, b, n, error)
  CALL CPU_TIME(stop_time)

  ! Did it work?
  IF (error == -1) THEN
     PRINT *, "Error in call to gaussian_solve"
  ELSE IF (error == -2) THEN
     PRINT *, "System is degenerate"
  ELSE
     PRINT *, " "
     PRINT *, "Solution is"
     PRINT '(1X,"x(",I4,") = ",F9.5)', (i, b(i), i = 1, n)
     PRINT *, 'Time = ', stop_time - start_time, ' seconds.'
  END IF

END PROGRAM solve_system
