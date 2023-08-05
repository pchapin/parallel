/* ***************************************************************************
FILE    : BoundedBuffer.java
SUBJECT : Java version of the bounded buffer problem using monitors.

This code is a lightly edited version of the example in "Real-Time Systems and Programming
Languages," fourth edition, by Alan Burns and Andy Wellings, Addison Wesley, ISBN =
978-0-321-41745-9. This sample is in section 5.9 of that book.

Notice that Java only has one condition variable per object (that variable is used implicitly by
wait(), notify(), and notifyAll()). If multiple conditions are needed they must be mutually
exclusive (this is the case here). However, if that is not the case notify(), etc, has no way to
indicate which condition is meant. See the reference above for why notifyAll() is used below and
not notify().

The "unchecked cast" warning can be removed by using an ArrayList<T> instead of an Object array.
The most natural thing to do would have been to declare buffer as type T[]. However, due to
type erasure Java does allow one to create an array (using new) with an element type that is a
generic parameter.

Send comments to: Peter C. Chapin <PChapin@vtc.vsc.edu>

*************************************************************************** */

public class BoundedBuffer<T> {
    private Object[] buffer;
    private int first = 0;
    private int last  = 0;
    private int count = 0;
    private int size;

    public BoundedBuffer(int length)
    {
        size   = length;
        buffer = new Object[length];
    }

    public synchronized void push(T item) throws InterruptedException
    {
        while (count == size) wait();
        last = (last + 1) % size;
        buffer[last] = item;
        count++;
        notifyAll();
    }

    public synchronized T pop() throws InterruptedException
    {
        while (count == 0) wait();
        first = (first + 1) % size;
        count--;
        notifyAll();
        return (T)buffer[first];
    }
}