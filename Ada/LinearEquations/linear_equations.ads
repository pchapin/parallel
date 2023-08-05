---------------------------------------------------------------------------
-- FILE    : linear_equations.ads
-- SUBJECT : Package of parallel linear algebra subprograms.
-- AUTHOR  : (C) Copyright 2010 by Peter C. Chapin
--
-- Please send comments or bug reports to
--
--      Peter C. Chapin <pcc482719@gmail.com>
---------------------------------------------------------------------------

generic
   type Floating_Type is digits <>;
package Linear_Equations is

   type Matrix is array(Positive range <>, Positive range <>) of Floating_Type;
   type Vector is array(Positive range <>) of Floating_Type;

   type System_Type(Size : Positive) is
      record
         A : Matrix(1 .. Size, 1 .. Size);
         B : Vector(1 .. Size);
      end record;

   type Error_Type is (None, Degenerate_System);

   procedure Gaussian_Solve
      (Equations : in out System_Type; Error : out Error_Type; Processor_Count : Positive);

end Linear_Equations;

