---------------------------------------------------------------------------
-- FILE       : bounded_buffers_test.adb
-- SUBJECT    : Sample program to exercise concurrent bounded buffers.
-- PROGRAMMER : (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
--
-- This program should not properly be called a test program. It's just a quick demonstration;
-- not any attempt at serious testing.
---------------------------------------------------------------------------

with Ada.Text_IO;         use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;

with Bounded_Buffers;

procedure Bounded_Buffers_Test is

   -- Instantiate the generic package.
   package Integer_Buffers is new Bounded_Buffers(Item_Type => Integer);

   -- Create a concurrent bounded buffer.
   Buffer : Integer_Buffers.Bounded_Buffer;

   -- Number of integers to pass between tasks.
   Limit : constant := 10_000;

   task Producer;  -- Trivial specifications.
   task Consumer;
   task Message_Server is
      entry Producer_Message(Number : Integer);
      entry Consumer_Message(Number : Integer);
   end Message_Server;

   task body Producer is
   begin
      for I in 0 .. Limit-1 loop
         Buffer.Push(I);
      end loop;
      Buffer.Push(-1);
      Message_Server.Producer_Message(Limit);
   end Producer;

   task body Consumer is
      I        : Integer;
      Expected : Integer := 0; -- At least do some minimal checking.
   begin
      loop
         Buffer.Pop(I);
         exit when I = -1;
         if I /= Expected then
            Put("CONSUMER ERROR! Expected: ");
            Put(Expected);
            Put("; Got: ");
            Put(I);
            New_Line;
         end if;
         Expected := Expected + 1;
      end loop;
      Message_Server.Consumer_Message(Expected);
   end Consumer;

   -- The purpose of this task is to serialize the output messages.
   task body Message_Server is
   begin
      loop
         select
            accept Producer_Message(Number : Integer) do
               Put("PRODUCER: Sent "); Put(Number, Width => 0); Put(" items.");
               New_Line;
            end Producer_Message;
         or
            accept Consumer_Message(Number : Integer) do
               Put("CONSUMER: Got "); Put(Number, Width => 0); Put(" items.");
               New_Line;
            end Consumer_Message;
         or
            terminate;
         end select;
      end loop;
   end Message_Server;

begin
   null;  -- Let the two tasks do their thing.
end Bounded_Buffers_Test;
