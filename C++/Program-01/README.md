# Problem 01

## Description

Implementing a Key-Value Store

Inspired by: [Implementing a Key-Value Store](https://codecapsule.com/2012/11/07/ikvs-implementing-a-key-value-store-table-of-contents/), and [Build Your Own Redis with C/C++](https://build-your-own.org/redis/#table-of-contents).

## Compiling

```bash
cmake -H. -Bbuild -G "Unix Makefiles"
cd build && make
./bin/db # Run server
./bin/client set key value # Run client
./bin/client get key  # Run client
./bin/client del key  # Run client
```

## To-Do

[ ] Use smart pointers
[ ] Refactor server parameters
[ ] Improve thread pool

## References

1 - [Implementing a Key-Value Store](https://codecapsule.com/2012/11/07/ikvs-implementing-a-key-value-store-table-of-contents/)
2 - [Beej's Guide to Network Programming](http://beej.us/guide/bgnet/)
