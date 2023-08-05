---------------------------------------------------------------------------
-- FILE       : bounded_buffers.adb
-- SUBJECT    : Implementation of a concurrent bounded buffer type using protected objects.
-- PROGRAMMER : (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
--
---------------------------------------------------------------------------

package body Bounded_Buffers is

   protected body Bounded_Buffer is

      entry Push(Item : in Item_Type) when Empty_Slots > 0 is
      begin
         Buffer(Next_In) := Item;
         Next_In         := Next_In + 1;
         Filled_Slots    := Filled_Slots + 1;
         Empty_Slots     := Empty_Slots - 1;
      end Push;

      entry Pop(Item : out Item_Type) when Filled_Slots > 0 is
      begin
         Item         := Buffer(Next_Out);
         Next_Out     := Next_Out + 1;
         Filled_Slots := Filled_Slots - 1;
         Empty_Slots  := Empty_Slots + 1;
      end Pop;

      function Count return Natural is
      begin
         return Filled_Slots;
      end Count;

      procedure Clear is
      begin
         Next_In      := 0;
         Next_Out     := 0;
         Filled_Slots := 0;
         Empty_Slots  := Maximum_Size;
      end Clear;

   end Bounded_Buffer;

end Bounded_Buffers;
