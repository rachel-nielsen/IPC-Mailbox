# IPC-Mailbox
## Source Files:
- mailbox.h
- mailbox.c
- mboxtest.c
- README.md
## Known Errors:
## References:
- https://forum.arduino.cc/t/how-to-elegantly-inherit-from-struct-overriding-adding-fields/1282346
- https://stackoverflow.com/questions/8276162/copy-blocks-of-memory-to-another-part-of-memory
- https://en.cppreference.com/w/cpp/string/byte/memcpy.html
- https://en.cppreference.com/w/c/types/size_t.html
- https://forum.arduino.cc/t/casting-type-void-to-uint8-t/1020173/2 
- shm-posix-producer.c
- shm-posix-consumer.c
- https://www.geeksforgeeks.org/c/c-arrays/
- https://www.w3schools.com/c/c_data_types_sizeof.php
- https://www.geeksforgeeks.org/c/fork-system-call/
- https://stackoverflow.com/questions/29635109/c-programming-fork-and-ipc-with-pipes#:~:text=There%20is%20a%20significant%20difference,Go%20to%20(1)
- https://stackoverflow.com/questions/22007031/parent-and-child-of-fork-in-c 
- https://www.w3schools.com/c/ref_stdio_printf.php#:~:text=Definition%20and%20Usage,a%20file%20or%20other%20location.
- https://stackoverflow.com/questions/13454036/how-to-clear-memory-contents-in-c#:~:text=3%20Answers&text=Be%20careful!,memory%20to%20zero%20as%20well.
- https://en.cppreference.com/w/cpp/string/byte/memset.html
- https://stackoverflow.com/questions/9923495/undefined-reference-shm-open-already-add-lrt-flag-here
## Terminal
### Compile
~~~
gcc mboxtest.c mailbox.c -o mboxtest
~~~
#### ...in Docker
~~~
gcc mboxtest.c mailbox.c -o mboxtest -lrt
~~~
### Run
~~~
./mboxtest
~~~
## Design Approach
Implemented an inter-process communication (IPC) through a mailbox/port stored in a shared memory with send and receive capabilities. The messages were stored in the shared memory in an array using bounded buffer/circular queue logic to send and receive messages in FIFO ordering. The sender and receiver were implemented with a fork with the sender as the parent process and the receiver as the child process. The functions used within these processes to send and receive messages can detect an empty mailbox, if the head is equal to the tail of the mailbox, and a full mailbox, if the tail is equal to the second to last slot in the array, returning an error value immediately, to avoid blocking communication. This implementation assumes that there is only one sender and one receiver.
