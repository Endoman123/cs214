# ++mymalloc()
Create an improved version of malloc() and free() that is smarter and more efficient.

## Requirements
This project required us to implement a function for both malloc() and free() that would 
manage the allocations on a static 4096-byte block. Blocks must be allocated on a first-fit
basis. All data on the block was to be considered contiguous.

## Implementation
In order to keep track of each block's size and state of use, metadata was required to be encoded along with
the userdata. Our metadata was encoded in 2 bytes using a structure representing metadata, using bitfields to
size the fields to the necessary number of bits that we needed.
### Metadata breakdown
```
 1   2        3
[1][111][111111111111]
```
1. In-use flag (1 bit)
2. Metadata identifier (3 bits)
3. Userdata size (12 bits)

Allocations in the static block are never deleted, only marked in use when malloc()'d or marked not in use when free()'d.
In order to represent this, 1 bit was used as a flag to mark the allocation either in use (1) or not in use (0).
Userdata size needed to be represented as a number between 1-4094, since users could not make 0-byte allocations
and the static block is initialized as one large allocation, with two bytes reserved for metadata. To represent this number,
we needed 12 bits. THe remaining 3 bits contain an identification combination to mark metadata from userdata.

## Memory Grind tests
