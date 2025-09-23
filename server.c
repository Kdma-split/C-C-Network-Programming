#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "socketutil.h"

int main() {
    int serverSocketFD = createTCPIpv4Socket();
    if (serverSocketFD < 0) {
        perror("socket");
        return 1;
    }

    int port = 5000;
    // struct sockaddr_in serverAddress = createIpv4Address("127.0.0.1", 6000);
    // struct sockaddr_in serverAddress = createIpv4Address("127.0.0.1", 5000);
    // struct sockaddr_in serverAddress = createIpv4Address("192.0.0.1", 5000);
    struct sockaddr_in serverAddress = createIpv4Address("192.168.49.1", port);

    if (bind(serverSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind");
        close(serverSocketFD);
        return 1;
    }
    printf("Socket successfully bound!\n");

    if (listen(serverSocketFD, 10) < 0) {
        perror("listen");
        close(serverSocketFD);
        return 1;
    }
    printf("Listening on port %d...\n", port);

    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocketFD < 0) {
        perror("accept");
        close(serverSocketFD);
        return 1;
    }
    printf("Client connected!\n");

    char buffer[1024];
    
    // while (true) {
    //     ssize_t amountReceived = recv(clientSocketFD, buffer, sizeof(buffer) - 1, 0);
    //     if (amountReceived > 0) {
    //         buffer[amountReceived] = '\0';
    //         printf("Client says: %s", buffer);
    //     } else if (amountReceived == 0) {
    //         printf("Client disconnected.\n");
    //         break;
    //     } else {
    //         perror("recv");
    //         break;
    //     }
    // }

// ==============  SOME OPTIMIZATIONS  ==================

    // Tune receive buffer (affects rwnd advertised to peer)
    int rcvbuf = 1024 * 64;  // 64KB
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

    // Force immediate ACKs
    int quickack = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &quickack, sizeof(quickack));

    // Enable TCP keepalive
    
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable)) < 0) {
        perror("setsockopt(SO_KEEPALIVE)");
        return 1;
    }
    // Override system defaults per socket
    int idle = 60;   // Start probing after 60s idle
    int interval = 10; // Interval between probes
    int maxpkt = 5;    // Probes before killing

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) < 0) {
        perror("setsockopt(TCP_KEEPIDLE)");
        return 1;
    }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) < 0) {
        perror("setsockopt(TCP_KEEPINTVL)");
        return 1;
    }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(maxpkt)) < 0) {
        perror("setsockopt(TCP_KEEPCNT)");
        return 1;
    }

    printf("TCP keepalive enabled (idle=%ds, interval=%ds, probes=%d)\n",
           idle, interval, maxpkt);


    while (true) {
        ssize_t amountReceived = recv(clientSocketFD, buffer, sizeof(buffer) - 1, 0);
        if (amountReceived > 0) {
            buffer[amountReceived] = '\0';
            printf("Client says: %s", buffer);

            // Echo back
            send(clientSocketFD, buffer, strlen(buffer), 0);
        } else if (amountReceived == 0) {
            printf("Client disconnected.\n");
            break;
        } else {
            perror("recv");
            break;
        }
    }


    close(clientSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);
    close(serverSocketFD);

    return 0;
}
