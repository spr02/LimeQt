#ifndef RING_BUF_SPSC
#define RING_BUF_SPSC

#include <atomic>
#include <cinttypes>
#include <cstring>
#include <iostream>

#define BUFFER_SIZE 65536//32768 //8192
#define MASK (BUFFER_SIZE - 1)

template <typename T>
class RingBufferSPSC
{
public:
    RingBufferSPSC()
    {
        head.store(0);
        tail.store(0);
    }
    
    ////////////// PUSH //////////////
    bool try_push(T const val) //non-blocking push
    {
        const uint64_t current_head = head.load();
        const uint64_t next_head = current_head + 1;
        if (current_head - tail.load(std::memory_order_acquire) <= BUFFER_SIZE-1 ) // syncronises with tail.store in pop thread
        {
            buffer[current_head & MASK] = val;
            head.store(next_head, std::memory_order_release);
            return true;
        }

        return false;
    }

    void push(T val) //blocking push
    {
        while( ! try_push(val) );
    }

    ////////////// POP ///////////////
    bool try_pop(T* const val) //non-blocking pop
    {
        const uint64_t current_tail = tail.load();
        if (current_tail == head.load(std::memory_order_acquire))
        {
            return false;
        }
        *val = buffer[current_tail & MASK];
        tail.store(current_tail + 1, std::memory_order_release); // syncronises with tail.load in push thread

        return true;
    }

    T pop() //blocking pop
    {
        T val;
        while( ! try_pop(&val) );
        return val;
    }

    ////////////// getter for head and tail //////////////
    uint64_t get_head (void)
    {
        return head.load();
    }

    uint64_t get_tail (void)
    {
        return tail.load();
    }
        
private:

    uint64_t buf_available_read (void)
    {
        return head.load(std::memory_order_acquire) - tail.load();
    }

    uint64_t buf_available_write (void)
    {
        return BUFFER_SIZE - head.load() - tail.load(std::memory_order_acquire);
    }
    
    //head and tail pointer
    std::atomic<uint64_t> head;
    std::atomic<uint64_t> tail;

    //actual buffer
    T buffer[BUFFER_SIZE];

};
#endif
