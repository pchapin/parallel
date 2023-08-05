---------------------------------------------------------------------------
-- FILE       : bounded_buffers.ads
-- SUBJECT    : Specification of a concurrent bounded buffer type using tasks.
-- PROGRAMMER : (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
--
---------------------------------------------------------------------------

generic
   type Item_Type is private;
package Bounded_Buffers is

   task type Bounded_Buffer is
      entry Push(Item : in Item_Type);
      entry Pop (Item : out Item_Type);
   end Bounded_Buffer;

end Bounded_Buffers;
