# ++mymalloc()
Create an improved version of malloc() and free() that is smarter and more efficient.

## Requirements
This project required a new implementation for both malloc() and free() that would manage the allocations on a static 4096-byte block. Blocks must be allocated on a first-fit basis. All data on the block was to be considered contiguous.

## Design

### MyMalloc() 
Mymalloc() uses a first-fit algorithm combined with a metadata struct to identify which blocks to return to the user given a size.



The metadata struct uses native C syntax called “bit fielding” to designate its fields as taking up a given width of bits instead of the full number of bytes that their datatype denotes. This is functionally the same within our program as encoding bits into a datatype and reading them using bitwise operations. 12 bits are used for storing the size of the block it represents up to 4096. Whether the block is in use is represented with 1 bit, 0 for not in use and 1 for in use. The last 3 bits of the 2 byte metadata is used an identifier to distinguish metadata from random floating bits, which for each metadata is set to a constant equal to 6.



Mymalloc() checks to see if it is the first time it is being run by checking the first 2 bytes of the static block is a valid metadata by checking the identifier. If it is valid, the function runs as normal. If it is not, then it is assumed that it is the first time mymalloc() is being run, and the block is initialized with the metadata representing the entire block.



After checking to see it is the first time being run, mymalloc() iterates through the block by adding the current address (i.e. the current offset in the block), the size of the metadata, and the size of the block the metadata represents to get to the location of the next metadata. As long as the operations performed in mymalloc() are mathematically correct, these metadata locations are guaranteed to be valid. Mymalloc() stops iterating when it reaches the first block that is both not in use and is equal to or bigger than the size requested by the user. If the block is bigger than the requested size + the size of the metadata, the block is split into two separate blocks each with their own metadata before the block that is exactly the size the user requested is returned. If it is not bigger than requested size by at least the size of metadata, the whole block is returned instead. If a block big enough does not exist, an error is thrown and NULL is returned instead.



### MyFree()
Myfree() works by performing address arithmetic to get to the metadata that the given pointer is represented by and changing its inUse bit to 0 after performing a number of error checks. Myfree() checks to see if the pointer is a proper pointer by checking if it is NULL, if the pointer was allocated by mymalloc() by checking to see if the difference between the address of the pointer and the address of the head of the static block is between 0 and the length of the block, and if the block was already freed by checking if the inUse bit is already 0.



After myfree() frees the pointer, it runs a helper function coaleseFreeBlocks(). This helper function iterates through the block’s metadata the same way mymalloc() does in its search for a block to allocate to the use. When it finds a chain of blocks that are not in use, it takes all of the chain and combines them into one block with one metadata. It looks for chains until it reaches the end of the block.



## Time Analysis
The new implementations were put under 6 different workloads:
<ol type="A">
  <li>Malloc and free 1 byte 150 times</li>
  <li>Malloc 1 byte 150 times, store in array, free every 50.</li>
  <li>Randomly malloc or free 1 byte</li>
  <li>Randomly malloc a random number of bytes or free</li>
  <li>Break memory into blocks, free random chunk of blocks, reallocate with one block</li>
  <li>Randomly allocate, randomly free, randomly reallocate</li>
</ol>



Each workload was repeated 100 times. Average run times (in microseconds) are posted below:

| Workload | ++malloc()  | System Impl |
|----------|-------------|-------------|
| A        | 6.37        | 2.48        |
| B        | 60.89       | 2.75        |
| C        | 7.36        | 2.68        |
| D        | 8.16        | 3.45        |
| E        | 2.66        | 0.61        |
| F        | 11.72       | 3.66        |

Most workloads run 2-3x slower using mymalloc() and myfree(). This slowdown is mostly seen with workloads B and F, showing almost a 4x decrease in speed from the system implementation. This can be attributed to the coalescion of free blocks in order to optimize the allocation algorithm. Workload C ran nearly the same between the two implementations of malloc() and free(); this workload, however, is extremely random and testing has shown that the runtimes can vary between 2.00-35.00 microseconds for the ++malloc() implentation, and 2.00-15.00 seconds for the system implementation. The overhead for mymalloc() and myfree() could be caused by the breakup and coalescion of allocations after those function calls.
