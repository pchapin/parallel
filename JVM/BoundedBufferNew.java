/* ***************************************************************************
FILE    : BoundedBufferNew.java
SUBJECT : Java version of the bounded buffer problem using monitors.

This code is a lightly edited version of the example in "Real-Time Systems and Programming
Languages," fourth edition, by Alan Burns and Andy Wellings, Addison Wesley, ISBN =
978-0-321-41745-9. This sample is in section 5.9 of that book.

This version makes use of concurrency supporting packages in the newer versions of the Java
library.

The "unchecked cast" warning can be removed by using an ArrayList<T> instead of an Object array.
The most natural thing to do would have been to declare buffer as type T[]. However, due to
type erasure Java does allow one to create an array (using new) with an element type that is a
generic parameter.

Send comments to: Peter C. Chapin <PChapin@vtc.vsc.edu>

*************************************************************************** */
import java.util.concurrent.locks.*;

public class BoundedBufferNew<T> {
    private Object[] buffer;
    private int first = 0;
    private int last  = 0;
    private int count = 0;
    private int size;

    private       Lock      lock;
    private final Condition notFull;
    private final Condition notEmpty;

    public BoundedBufferNew(int length)
    {
        size     = length;
        buffer   = new Object[length];
        lock     = new ReentrantLock();
        notFull  = lock.newCondition();
        notEmpty = lock.newCondition();
    }

    public void push(T item) throws InterruptedException
    {
        lock.lock();
        try {
            while (count == size) notFull.await();
            last = (last + 1) % size;
            buffer[last] = item;
            count++;
            notEmpty.signal();
        }
        finally {
            lock.unlock();
        }
    }

    public T pop() throws InterruptedException
    {
        lock.lock();
        try {
            while (count == 0) notEmpty.await();
            first = (first + 1) % size;
            count--;
            notFull.signal();
            return (T)buffer[first];
        }
        finally {
            lock.unlock();
        }
    }
}