# Problem 08

## Description

Beej's Guide to Network Programming code.

## OSI model

||Layer|
|-|-|
|7|Application|
|6|Presentation|
|5|Session|
|4|Transport|
|3|Network|
|2|Data link|
|1|Physical|

## Sockets

 - **Stream Sockets**: SOCK_STREAM
 - **Datagram Sockets**: SOCK_DGRAM

Note: HTTP uses stream sockets


## 

The sequence of system calls when you're going to be listening for incoming connections:

1. getaddrinfo();
1. socket();
1. bind();
1. listen();
1. accept();

## References

1 - [Beej's Guide to Network Programming](http://beej.us/guide/bgnet/)

2 - [OSI model](https://en.wikipedia.org/wiki/OSI_model)