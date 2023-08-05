---------------------------------------------------------------------------
-- FILE       : philosophers.adb
-- SUBJECT    : Ada solution to the Dining Philosophers Problem.
-- PROGRAMMER : (C) Copyright 2008 by Peter C. Chapin
--
-- This version solves the problem using entry families. A better approach is to use the
-- requeue facility. I leave it as an exercise for the reader to rewrite this program
-- in that way.
---------------------------------------------------------------------------
with Ada.Text_IO;
use Ada.Text_IO;

procedure Philosophers is
   type Counter_Type is mod 5;
   type Flags_Type is array(Counter_Type) of Boolean;

   package Counter_IO is new Ada.Text_IO.Modular_IO(Counter_Type);
   use Counter_IO;

   -- The message server coordinates non-atomic message output.
   task Message_Server is
      entry Eat_Message(P : in Counter_Type);
   end Message_server;

   task body Message_Server is
   begin
      loop
         select
            accept Eat_Message(P : in Counter_Type) do
               Put("Eating: "); Put(P, 0); New_Line;
            end;
         or
            terminate;
         end select;
      end loop;
   end Message_Server;

   -- Procedure to simulate the thinking process.
   procedure Think(P : Counter_Type) is
   begin
      null;
   end Think;

   -- Protected type to coordinate access to the forks.
   protected type Fork_Manager is
      entry Take_Forks(Counter_Type);
      procedure Put_Forks(P : in Counter_Type);
   private
      Fork_Available : Flags_Type := (others => True);
   end Fork_Manager;

   protected body Fork_Manager is
      entry Take_Forks(for P in Counter_Type)
        when Fork_Available(P) and Fork_Available(P+1) is
      begin
         Fork_Available(P) := False;
         Fork_Available(P+1) := False;
      end Take_Forks;

      procedure Put_Forks(P : in Counter_Type) is
      begin
         Fork_Available(P) := True;
         Fork_Available(P+1) := True;
      end Put_Forks;
   end Fork_Manager;

   Forks : Fork_Manager;

   task type Philosopher is
      entry Initialize(P : in Counter_Type);
   end Philosopher;

   task body Philosopher is
      P_Number : Counter_Type;
   begin
      accept Initialize(P : in Counter_Type) do
         P_Number := P;
      end Initialize;
      loop
         Think(P_Number);
         Forks.Take_Forks(P_Number);
         Message_Server.Eat_Message(P_Number);
         Forks.Put_Forks(P_Number);
      end loop;
   end;

   Table : array(Counter_Type) of Philosopher;

begin
   for I in Counter_Type loop
      Table(I).Initialize(I);
   end loop;
end Philosophers;
