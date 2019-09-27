# RingBuffer
A ring/cyclic buffer suitable for ISR-Task communication

![Ring Buffer, Source: Wikipedia](https://upload.wikimedia.org/wikipedia/commons/thumb/d/d9/Ring_buffer.svg/246px-Ring_buffer.svg.png)

A C++ 17, templated and stack allocated ring buffer for continuous i/o. 


### Notes
Instead of releasing old data, you handle the buffer overrun.
To use with an RTOS, ensure there is a semaphore lock before calling ReadNext until an IRQ that calls WriteNext to the ring buffer occurs and releases the semaphore lock for reading. 



### Dependencies
- libstdc++17
- libpthread
- ninja
- googletest (testing)

### Running Tests

```bash
user@linux:~$ ninja tests.bin
user@linux:~$ ./tests.bin
```
