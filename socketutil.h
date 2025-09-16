#ifndef SOCKETUTIL_H
#define SOCKETUTIL_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Create a TCP/IPv4 socket
static inline int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

// Create an IPv4 address (stack-based, not malloc)
static inline struct sockaddr_in createIpv4Address(const char *ip, int port) {
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (ip == NULL || strlen(ip) == 0) {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, ip, &address.sin_addr);
    }
    return address;
}

#endif
