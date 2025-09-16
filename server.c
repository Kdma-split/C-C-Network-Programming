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
    while (true) {
        ssize_t amountReceived = recv(clientSocketFD, buffer, sizeof(buffer) - 1, 0);
        if (amountReceived > 0) {
            buffer[amountReceived] = '\0';
            printf("Client says: %s", buffer);
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
