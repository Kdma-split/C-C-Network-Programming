// #include <stdio.h>
// #include <stdbool.h>
// #include <unistd.h>
// #include "socketutil.h"

// int main() {
//     int serverSocketFD = createTCPIpv4Socket();
//     if (serverSocketFD < 0) {
//         perror("socket");
//         return 1;
//     }

//     struct sockaddr_in serverAddress = createIpv4Address("127.0.0.1", 6000);

//     if (bind(serverSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
//         perror("bind");
//         close(serverSocketFD);
//         return 1;
//     }
//     printf("Socket successfully bound!\n");

//     if (listen(serverSocketFD, 10) < 0) {
//         perror("listen");
//         close(serverSocketFD);
//         return 1;
//     }
//     printf("Listening on port 6000...\n");

//     struct sockaddr_in clientAddress;
//     socklen_t clientAddressSize = sizeof(clientAddress);

//     int clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);
//     if (clientSocketFD < 0) {
//         perror("accept");
//         close(serverSocketFD);
//         return 1;
//     }
//     printf("Client connected!\n");

//     char buffer[1024];
//     while (true) {
//         ssize_t amountReceived = recv(clientSocketFD, buffer, sizeof(buffer) - 1, 0);
//         if (amountReceived > 0) {
//             buffer[amountReceived] = '\0';
//             printf("Client says: %s", buffer);
//         } else if (amountReceived == 0) {
//             printf("Client disconnected.\n");
//             break;
//         } else {
//             perror("recv");
//             break;
//         }
//     }

//     close(clientSocketFD);
//     shutdown(serverSocketFD, SHUT_RDWR);
//     close(serverSocketFD);

//     return 0;
// }












#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "socketutil.h"

int main() {
    int sockFD = createTCPIpv4Socket();
    if (sockFD < 0) {
        perror("socket");
        return 1;
    }

    // connect to server running on 127.0.0.1:5000
    // struct sockaddr_in serverAddress = createIpv4Address("127.0.0.1", 5000);
    struct sockaddr_in serverAddress = createIpv4Address("192.168.49.1", 5000);

    if (connect(sockFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("connect");
        close(sockFD);
        return 1;
    }
    printf("Connected to server!\n");

    char buffer[1024];
    while (true) {
        printf("You: \n");
        if (!fgets(buffer, sizeof(buffer), stdin)) break;
        if (strcmp(buffer, "exit\n") == 0) break;

        send(sockFD, buffer, strlen(buffer), 0);

        ssize_t n = recv(sockFD, buffer, sizeof(buffer)-1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Server: %s", buffer);
        } else if (n == 0) {
            printf("Server closed connection.\n");
            break;
        } else {
            perror("recv");
            break;
        }
    }

    close(sockFD);
    return 0;
}
