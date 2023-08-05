---------------------------------------------------------------------------
-- FILE       : find.adb
-- SUBJECT    : Parallel Find in Ada.
-- PROGRAMMER : (C) Copyright 2010 by Peter C. Chapin <PChapin@vtc.vsc.edu>
--
-- For reasons that are not entirely clear to me, this program fails with a stack overflow.
-- It does not seem to me that Parallel_Integer_Find requires that much stack space. Could
-- it be some problem with GNAT not recovering memory properly? That seems unlikely, but it
-- might be possible.
---------------------------------------------------------------------------
with Ada.Calendar;
with Ada.Text_IO;
with Ada.Float_Text_IO;
with Ada.Integer_Text_IO;
with Ada.Unchecked_Deallocation;

-- Unlike the C++ version, this version is not generic. This should be fixed at some point.
procedure Find is
   use type Ada.Calendar.Time;
   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);

   type Workspace_Type is array(Natural range <>) of Integer;
   
   -- Serial version of a function that searches an array of integers.
   function Serial_Integer_Find(Sequence : Workspace_Type; Item : Integer) return Natural is
      Index : Natural := Sequence'Last + 1;
   begin
      for I in Sequence'Range loop
         if Sequence(I) = Item then
            Index := I;
            exit;
         end if;
      end loop;
      return Index;
   end Serial_Integer_Find;
   
   
   -- Parallel version of a function that searches an array of integers.
   function Parallel_Integer_Find(Sequence : Workspace_Type; Item : Integer) return Natural is
      
      -- Pass the range to search into the task as discriminants.
      task type Find_Helper(First : Natural; Last : Natural) is
         entry Get_Result(Result : out Natural);
      end Find_Helper;
      
      task body Find_Helper is
         I : Natural;
      begin        
         -- When a while loop is used with serial find the performce is only about 1/2. GNAT
         -- appears to be able to optimize a for loop more aggressively. It would be a shame
         -- to loose any parallel advantage over an optimization issue such as this.
         --
         I := First;
         while I <= Last loop
            exit when Sequence(I) = Item;
            I := I + 1;
         end loop;
         
         accept Get_Result(Result : out Natural) do
            Result := I;
         end Get_Result;
      end Find_Helper;
      
      type Find_Helper_Access is access Find_Helper;
      procedure Find_Helper_Deallocate is
         new Ada.Unchecked_Deallocation(Find_Helper, Find_Helper_Access);
      
      -- For now just create two tasks.
      Finders   : array(1 .. 2) of Find_Helper_Access;
      Step_Size : Natural;
      Result    : Natural;
   begin
      -- This implementation is incomplete. It is intended to just be enough to explore the
      -- required execution time. It needs to be finished before it will produce correct
      -- results.
      --
      Step_Size := (Sequence'Last - Sequence'First) / 2;
      for I in 1 .. 2 loop
         Finders(I) :=
            new Find_Helper(Sequence'First + (I - 1)*Step_Size, Sequence'First + I*Step_Size);
      end loop;
      for I in reverse 1 .. 2 loop
         Finders(I).Get_Result(Result);
         -- Race condition? Could Finders(I) still be executing when we deallocate?
         Find_Helper_Deallocate(Finders(I));
      end loop;
      return Sequence'Last + 1;
   end Parallel_Integer_Find;
   
   
   Size : Natural;

begin -- Find
   
   Size := 500_000;
   while Size <= 3_000_000 loop

      declare
         Workspace         : Workspace_Type(0 .. Size - 1);
         Start_Time        : Ada.Calendar.Time;
         Stop_Time         : Ada.Calendar.Time;
         Serial_Duration   : Duration;
         Parallel_Duration : Duration;
         Speedup           : Float;
      begin
         -- Fill the array.
         for I in Workspace'Range loop
            Workspace(I) := I;
         end loop;
         
         -- Search the array 1000 times so it takes long enough to get a good measurement.
         Start_Time := Ada.Calendar.Clock;
         for I in 1 .. 1000 loop
            if Serial_Integer_Find(Workspace, Size) /= Size then
               Ada.Text_IO.Put_Line("Serial_Integer_Find unexpectedly finds search value!");
            end if;
         end loop;
         Stop_Time := Ada.Calendar.Clock;
         Serial_Duration := Stop_Time - Start_Time;
         
         -- Search the array 1000 times to it takes long enough to get a good measurement.
         Start_Time := Ada.Calendar.Clock;
         for I in 1 .. 1000 loop
            if Parallel_Integer_Find(Workspace, Size) /= Size then
               Ada.Text_IO.Put_Line("Parallel_Integer_Find unexpectedly finds search value!");
            end if;
         end loop;
         Stop_Time := Ada.Calendar.Clock;
         Parallel_Duration := Stop_Time - Start_Time;

         -- Display the results of this run. This table needs to be formatted with headings.
         Ada.Integer_Text_IO.Put(Size);
         Duration_IO.Put(Serial_Duration);
         Duration_IO.Put(Parallel_Duration);
         Speedup := Float(Serial_Duration) / Float(Parallel_Duration);
         Ada.Float_Text_IO.Put(Speedup);
         Ada.Text_IO.New_Line;
      end;      

      Size := Size + 100_000;
   end loop;
   
end Find;

