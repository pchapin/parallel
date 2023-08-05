---------------------------------------------------------------------------
-- FILE    : linear_equations.adb
-- SUBJECT : Implementation of parallel linear algebra subprograms.
-- AUTHOR  : (C) Copyright 2010 by Peter C. Chapin
--
-- Please send comments or bug reports to
--
--      Peter C. Chapin <pcc482719@gmail.com>
---------------------------------------------------------------------------

package body Linear_Equations is

   procedure Elimination
      (Equations : in out System_Type; Error : out Error_Type; Processor_Count : Positive) is
      Size       : Positive renames Equations.Size;
      A          : Matrix   renames Equations.A;
      B          : Vector   renames Equations.B;

      task type Row_Processor_Type is
         entry Work_Unit(Base_Row, Start_Row, Stop_Row : Positive);
         entry Wait;
      end Row_Processor_Type;

      task body Row_Processor_Type is
         Local_Base_Row  : Positive;
         Local_Start_Row : Positive;
         Local_Stop_Row  : Positive;
         Temp : Floating_Type;
      begin
         loop
            -- Find out what work we are supposed to do.
            select
                  accept Work_Unit(Base_Row, Start_Row, Stop_Row : Positive) do
                     Local_Base_Row  := Base_Row;
                     Local_Start_Row := Start_Row;
                     Local_Stop_Row  := Stop_Row;
                  end Work_Unit;
               or
                  terminate;
            end select;

            -- Subtract multiples of row Local_Base_Row from the rows in our work unit.
            for J in Local_Start_Row .. Local_Stop_Row loop
               Temp := A(J, Local_Base_Row) / A(Local_Base_Row, Local_Base_Row);
               for K in 1 .. Size loop
                  A(J, K) := A(J, K) - (Temp * A(Local_Base_Row, K));
               end loop;
               B(J) := B(J) - (Temp * B(Local_Base_Row));
            end loop;

            -- Wait until the main task tells us to continue.
            accept Wait;
         end loop;
      end Row_Processor_Type;

      -- Variables used during the preparation of the work units.
      K       : Positive;
      Temp    : Floating_Type;
      Maximum : Floating_Type;

      -- To coordinate the multiple processors.
      Processors         : array(1 .. Processor_Count) of Row_Processor_Type;
      Rows_Per_Processor : Natural;   -- Zero rows per processor is possible at the end.
      Start_Row          : Positive;
      Stop_Row           : Positive;

   begin -- Elimination
      Error := None;

      for I in 1 .. Size loop

         -- Find the row with the largest value of |A(J, I)|, J in I .. Size
         K := I;
         Maximum := abs A(I, I);
         for J in I + 1 .. Size loop
            if abs A(J, I) > Maximum then
               K := J;
               Maximum := abs A(J, I);
            end if;
         end loop;

        -- Check for |A(K, I)| zero.
        if abs A(K, I) < 1.0E-5 then
           Error := Degenerate_System;
           return;
        end if;

        -- Exchange row I and row K, if necessary.
        if I /= K then
           for J in 1 .. Size loop
              Temp    := A(I, J);
              A(I, J) := A(K, J);
              A(K, J) := Temp;
           end loop;

           -- Exchange corresponding elements of B.
           Temp := B(I);
           B(I) := B(K);
           B(K) := Temp;
        end if;

        -- Assign work to the available processors.
        Rows_Per_Processor := (Size - I) / Processor_Count;
        for Processor_Number in 1 .. Processor_Count loop
           Start_Row := I + 1 + (Processor_Number - 1)*Rows_Per_Processor;
           if Processor_Number /= Processor_Count then
              Stop_Row := I + Processor_Number*Rows_Per_Processor;
           else
              Stop_Row := Size;
           end if;

           Processors(Processor_Number).Work_Unit(I, Start_Row, Stop_Row);
        end loop;

        -- Wait for the processors to finish.
        for Processor_Number in 1 .. Processor_Count loop
           Processors(Processor_Number).Wait;
        end loop;
      end loop;
   end Elimination;


   procedure Back_Substitution(Equations : in out System_Type; Error : out Error_Type) is
      Size       : Positive renames Equations.Size;
      A          : Matrix   renames Equations.A;
      B          : Vector   renames Equations.B;

      Sum        : Floating_Type;
   begin
      Error := None;

      for I in reverse 1 .. Size loop
         if abs A(I, I) <= 1.0E-5 then
            Error := Degenerate_System;
            return;
         end if;

         Sum := B(I);
         for J in I + 1 .. Size loop
            Sum := Sum - (A(I, J) * B(J));
         end loop;
         B(I) := Sum / A(I, I);
      end loop;
   end Back_Substitution;


   procedure Gaussian_Solve
      (Equations : in out System_Type; Error : out Error_Type; Processor_Count : Positive) is
   begin
      Elimination(Equations, Error, Processor_Count);
      if Error = None then
         Back_Substitution(Equations, Error);
      end if;
   end Gaussian_Solve;

end Linear_Equations;
