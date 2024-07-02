Understaning how software design decisions those that lead to the socket interface for
TCP/IP protocol layer, and how it is abstracted by this interface to the application
programmer as system calls for communication rather than for local device/file access. 
System calls add another layer of abstraction for making applications programs usiing
socket interface portable across Operating Systems that support the socket interface.

The notion of 'Internet' which abstracts away all disparate packet switched hardware 
architectures into a set of common protocols through which all computers are able to
stay connected and communicate universally despite numerous communications hardware.

The socket abstraction allows (within socket system call, 3rd arg) a set of several 
protocols in each protocol family (1st argument) which provides either stream type
service or datagram type (2nd argument).

Sockets provide I/O over the network either UDP or TCP, that is either blocked/timed,
non-blocked/checkingOrSleepPoll'd, multiplexed I/O, Signal I/O, Asynchronous I/O which
is completely non-blocked independent of underlying network transmission taking place
between communicating peers. System wide Non-blocking option can be used while socket
creation, to avoid that use socket option for one time non-blocking RD/WR operations.

The TCP/IP protocol software (kernel) and POSIX socket API (system calls for apps), is
an software engineering beast that has to be mastered using client-server programming
techniques as specified in Comer/Stevens UNIX programming books.

WAN problems are lost packets or duplicate packets from either side between the 
communicating peers. The dalays and variation in delays are slight in LAN but high
in WAN. Thus the only solution to WAN problems is timeout and adaptive retransmissions
that are problems solved in the TCP/connection-oriented transport protocol/Layer4.
Use it, use UDP only for LAN which is sufficiently reliable, and for multicast/
broadcast.
