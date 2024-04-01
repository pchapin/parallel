---------------------------------------------------------------------------
-- FILE    : solve_system.adb
-- SUBJECT : Program to demonstrate parallel linear algebra subprograms.
-- AUTHOR  : (C) Copyright 2010 by Peter C. Chapin
--
-- Please send comments or bug reports to
--
--      Peter C. Chapin <pcc482719@gmail.com>
---------------------------------------------------------------------------
with Ada.Calendar;        use Ada.Calendar;
with Ada.Command_Line;    use Ada.Command_Line;
with Ada.Text_IO;         use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Linear_Equations;

procedure Solve_System is
   type Floating_Type is digits 8;
   package Equations   is new Linear_Equations(Floating_Type);
   package Floating_IO is new Float_IO(Floating_Type);
   package Duration_IO is new Fixed_IO(Duration);
   use Floating_IO;
   use Duration_IO;

   type System_Access is access Equations.System_Type;

   System_Definition_File : File_Type;
   Size                   : Positive;
   Current_System         : System_Access;
   Error_Status           : Equations.Error_Type;
   Start_Time, Stop_Time  : Time;
begin
   -- Check command line.
   if Ada.Command_Line.Argument_Count /= 1 then
      Put_Line("Error: Expecting the name of a system definition file.");
      Set_Exit_Status(Failure);
      return;
   end if;

   -- Read the system.
   Open(System_Definition_File, In_File, Ada.Command_Line.Argument(1));
   Get(System_Definition_File, Size);
   Current_System := new Equations.System_Type(Size);
   for I in 1 .. Size loop
      for J in 1 .. Size loop
         Get(System_Definition_File, Current_System.A(I, J));
      end loop;
      Get(System_Definition_File, Current_System.B(I));
   end loop;
   Close(System_Definition_File);

   -- Solve the system and display the results.
   Start_Time := Clock;
   Equations.Gaussian_Solve(Current_System.all, Error_Status, 2);
   Stop_Time := Clock;
   case Error_Status is
      when Equations.Degenerate_System =>
         Put_Line("System is degenerate.");
         Set_Exit_Status(Failure);

      when Equations.None =>
         New_Line;
         Put_Line("Solution is");
         for I in 1 .. Size loop
            Put("x("); Put(I, Width => 4); Put(") = "); Put(Current_System.B(I)); New_Line;
         end loop;
         Put("Execution time = "); Put(Stop_Time - Start_Time); Put(" seconds"); New_Line;
   end case;

exception
   when Name_Error =>
      Put_Line
         ("Error: Unable to open the system definition file: " & Ada.Command_Line.Argument(1));
      Set_Exit_Status(Failure);

end Solve_System;

