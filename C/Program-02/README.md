# Problem 02

## Description

Validation suggestion to check if malloc returned NULL in great article [Code memory safety and efficiency by example](https://opensource.com/article/21/8/memory-programming-c) written by [Marty Kalin](https://condor.depaul.edu/mkalin/) on [opensource.com](https://opensource.com).

In the section 'Heap storage', I noticed that the main function checks if the /\* malloc failed \*/, but since we are initializing *heap_nums in the same function (get_heap_array) we are trying to allocate memory, we need to add validation before trying to access the array to avoid segmentation failure (SIGSEGV) in case of allocation failure.²

## References

1 - [Code memory safety and efficiency by example](https://opensource.com/article/21/8/memory-programming-c)

2 - [My comment on the article](https://opensource.com/article/21/8/memory-programming-c#comment-217606)
