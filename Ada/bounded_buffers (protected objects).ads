---------------------------------------------------------------------------
-- FILE       : bounded_buffers.ads
-- SUBJECT    : Specification of a concurrent bounded buffer type using protected objects.
-- PROGRAMMER : (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
--
---------------------------------------------------------------------------

generic
   type Item_Type is private;
package Bounded_Buffers is
   
   Maximum_Size : constant := 8;
   type Buffer_Index is mod Maximum_Size;
   type Buffer_Type  is array(Buffer_Index) of Item_Type;

   protected type Bounded_Buffer is
      entry Push(Item : in Item_Type);
      entry Pop (Item : out Item_Type);

      function Count return Natural;
      procedure Clear;
   private
      Buffer       : Buffer_Type;
      Next_In      : Buffer_Index := 0;
      Next_Out     : Buffer_Index := 0;
      Filled_Slots : Natural := 0;
      Empty_Slots  : Natural := Maximum_Size;
   end Bounded_Buffer;

end Bounded_Buffers;
