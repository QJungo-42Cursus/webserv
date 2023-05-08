// Mainly following beej.us tutorial
// Use stream socket (SOCK_STREAM) for reliable two-way connection
// Can do some connection tests using nc (macos equiv of telnet)
// echo -n "GET / HTTP/1.0\r\n\r\n" | nc google.com 80
//
// Network Byte Order is Big Endian, and because different systems use different endianess, need
// to convert ints to Network order:
// Host byte order to Network byte order = hton + s (short int) or l (long int)
// A socket descriptor is a file descriptor (int fd)
/* struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};*/
// getaddrinfo() will fill most of the struct
/*int getaddrinfo(const char *restrict node,
                       const char *restrict service,
                       const struct addrinfo *restrict hints,
                       struct addrinfo **restrict res);*/


/*getaddrinfo() function allocates and initializes a linked
       list of addrinfo structures, one for each network address that
       matches node and service, subject to any restrictions imposed by
       hints, and returns a pointer to the start of the list in res.
       The items in the linked list are linked by the ai_next field.*/




// nb: in the above struct, there is a pointer to a struct sockaddr
// struct sockaddr holds  socket addr info for many types of sockets
/* struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
};*/

// For sa_family, we'll use AF_INET for IPv4 or AF_INET6 for IPv6
// sa_data contains dest address and port number for the socket
//
//To deal with struct sockaddr, common to use instead a struct sockaddr_in (in for internet) to be used with IPv4
//A pointer to sockaddr_in can then be cast to pointer to sockaddr (and vice versa)
//
//For IPv4:
/* (IPv4 only--see struct sockaddr_in6 for IPv6)

struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};*/

//sin_zero is used to zero-pad sockaddr_in to the mem size of sockaddr
// Internet address (a structure for historical reasons)
/*struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};*/

/*******************************************/


