---------------------------------------------------------------------------
-- FILE       : bounded_buffers.adb
-- SUBJECT    : Implementation of a concurrent bounded buffer type using tasks.
-- PROGRAMMER : (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
--
---------------------------------------------------------------------------

package body Bounded_Buffers is
   Maximum_Size : constant := 8;

   task body Bounded_Buffer is
      type Buffer_Index is mod Maximum_Size;

      Buffer : array(Buffer_Index) of Item_Type;
      Next_In      : Buffer_Index := 0;
      Next_Out     : Buffer_Index := 0;
      Filled_Slots : Natural := 0;
      Empty_Slots  : Natural := Maximum_Size;
   begin
      loop
         select
            when Empty_Slots > 0 =>
               accept Push(Item : in Item_Type) do
                  Buffer(Next_In) := Item;
                  Next_In := Next_In + 1;
                  Filled_Slots := Filled_Slots + 1;
                  Empty_Slots  := Empty_Slots - 1;
               end Push;
         or
            when Filled_Slots > 0 =>
               accept Pop(Item : out Item_Type) do
                  Item := Buffer(Next_Out);
                  Next_Out := Next_Out + 1;
                  Filled_Slots := Filled_Slots - 1;
                  Empty_Slots  := Empty_Slots + 1;
               end Pop;
         or
            terminate;
         end select;
      end loop;
   end Bounded_Buffer;

end Bounded_Buffers;
