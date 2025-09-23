#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "socketutil.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdlib.h>

struct AcceptedSocket *acceptIncomingConnection(int serverSockFD);

int main() {
    int serverSockFD = createTCPIpv4Socket
    struct sockaddr_in *serverAddress = createIpv4Address("", 5000);

    int result = bind(serverSockFD, serverAddress, sizeof(*serverAddress));
    if (result < 0) {
        perror("bind");
        close(serverSockFD);
        return 1;   
    }

    int listenResult = listen(serverSockFD, 10);
    if (listenResult < 0) {
        perror("listen");
        close(serverSockFD);
        return 1;
    }
    printf("Listening on port %d...\n", ntohs(serverAddress->sin_port));

    struct AcceptedSocket *clientSocket = acceptIncomingCnnection()
    
    if (!clientSocket->isAccepted) {
        perror("accept");
        close(serverSockFD);
        free(clientSocket);
        return 1;
    }
    printf("Client connected from %s:%d\n", inet_ntoa(clientSocket->addr.sin_addr.s_addr), ntohs(clientSocket->addr.sin_port));

    // MESSAGE EXCHANGE LOGIC / PACKET PARSING LOGICS...

    close(clientSocket->sockFD);
    shutudown(serverSocketFD, SHUT_RDWR);
    close(serverSocketFD);
    
    free(clientSocket);
    return 0;
}









// INSIDE sockrtutil.h: 

struct AcceptedSocket {
    int sockFD;
    struct sockaddr_in addr;
    int error;
    bool isAccepted;
};

struct AcceptedScoket *acceptIncomingConnection(int serverSockFD) {
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSockFD = accept(serverSockFD, clientAddress, clientAddressSize);
    
    // struct AcceptedSocket acceptedSockResult = {
    //     .sockFD = clientSockFD,
    //     .addr = clientAddress,
    //     .error = (clientSockFD < 0) ? errno : 0,
    //     .isAccepted = (clientSockFD >= 0)
    // };

    struct AcceptedSocket *accpetedSockResult = malloc(sizeof(struct AcceptedSocket));
    acceptedSockRes->sockFD = clientSockFD;
    acceptedSockRes->addr = clientAddress;
    acceptedSockRes->error = (clientSockFD < 0) ? errno : 0;
    acceptedSockRes->isAccepted = (clientSockFD >= 0);

    return acceptedSockResult;
}








// What exactly is difference between sin_addr.s_addr and inet_addr?
// addr.sin_addr.s_addr = inet_addr("127.0.0.1");
// is what i am using in my programming - what does this achieve?

// sin_addr is the IP address in the socket (the socket structure also contains other data, such as a port). The type of sin_addr is a union, so it can be accessed in three different ways : as s_un_b (four 1-byte integers), s_un_w (two 2-bytes integers) or as s_addr (one 4-bytes integer).
// inet_addr converts an IPv4 address from a string in dotted decimal representation to an integer. This function is deprecated because it does not support IPv6, use inet_pton instead.
// So basically, the line about which you are asking loads into the socket the IP address 127.0.0.1, meaning the local host.








char buffer[1024];

while (true) {
    ssize_t amountReceived = recv(clientSocketFD, buffer, sizeof(buffer) - 1, 0);
    
    if (amountReceived > 0) {
        buffer[amountReceived] = '\0';

        // ==================   IMPLEMENT THE SEGMENT PARSING LOGIC (LENGTH PREFIXED PROTOCOL)  ==================
        
        int startFrom = 0; // or any other index you want to check
        // int msgLen = strlen(buffer);
        char ch = buffer[startFrom];

        if (isdigit(ch)) {

            printf("'%c' is a digit.\n", ch);
        } else {
            printf("'%c' is not a digit.\n", ch);
        }
        
        // printf("Client says: %s", buffer);
    } 
    else if (amountReceived == 0) {
        printf("Client disconnected.\n");
        break;
    } 
    else {
        perror("recv");
        break;
    }
}











// ==============  SOME OPTIMIZATIONS  ==================

int rcvbuf = 1024 * 64;  // 64KB
setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)); // Tune receive buffer (affects rwnd advertised to peer)
int sndbuf = 1024 * 64;  // 64KB
setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf)); // Tune send buffer (affects cwnd growth)
int enable = 1;
setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)); // Disable Nagle's algorithm (send small packets immediately)
int quickack = 1;
setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &quickack, sizeof (quickack)); // Force immediate ACKs
int keepalive = 1;
setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)); // Enable TCP keepalive
int keepidle = 60; // seconds
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle)); // Start keepalive after 60s idle
int keepintvl = 10; // seconds
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl)); // Interval between keepalive probes
int keepcnt = 5; // probes
setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt)); // Number of probes before declaring the connection dead
int mss = 1460; // bytes
setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss)); // Set maximum segment size
int window_scale = 7; // shift count
setsockopt(sockfd, IPPROTO_TCP, TCP_WINDOW_CLAMP, &window_scale, sizeof(window_scale)); // Set window scale (affects rwnd and cwnd)
const char *cc_algo = "reno"; // or "cubic", "bbr" if available and privileged      
setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, cc_algo, strlen(cc_algo)); // Set congestion control algorithm
// Note: Some of these options may require elevated privileges or may not be supported on all systems
// Also, error checking is omitted for brevity. In production code, always check the return values of setsockopt.
// Remember to include necessary headers: <sys/socket.h>, <netinet/tcp.h>, <unistd.h>, <stdio.h>, <string.h>
// And link with -lpthread if using threads
// Adjust values according to your application's requirements and network conditions
// These settings can significantly affect performance, so test and tune them as needed
// Always consider the implications of changing TCP parameters, as they can lead to unexpected behavior if not used correctly
// For example, disabling Nagle's algorithm can increase the number of small packets sent, which may not be desirable in all scenarios
// Similarly, aggressive keepalive settings can lead to unnecessary network traffic
// Use these settings judiciously and monitor their impact on your application's performance and reliability
// Consult your system's documentation for more details on each option and its effects
// Some options may not be available on all platforms or may have different names or behaviors
// Always ensure that your application has the necessary permissions to set these options, especially for options that  
// affect system-wide behavior or require elevated privileges
// For example, setting the congestion control algorithm may require root privileges on some systems
// Always test your application under realistic network conditions to ensure that the chosen settings provide the desired performance
// and reliability
// Consider using tools like iperf or netperf to measure the impact of these settings on throughput and latency
// Remember that network conditions can vary widely, so what works well in one environment may not be optimal in another
// Regularly review and update your TCP settings as needed to adapt to changing network conditions and application requirements
// Stay informed about updates to TCP implementations and best practices in network programming
// Engage with the community and share your experiences to help others learn from your successes and challenges
// Always prioritize the reliability and stability of your application over aggressive performance tuning
// as overly aggressive settings can lead to instability and degraded performance
// Finally, document your TCP settings and the rationale behind them to aid future maintenance and troubleshooting efforts
// This documentation can be invaluable for understanding the impact of these settings on your application's behavior
// and for making informed decisions about future changes
// Always consider the security implications of your TCP settings, as some configurations may expose your application to
// potential vulnerabilities or attacks
// For example, overly aggressive keepalive settings could potentially be exploited for denial-of-service attacks
// Always balance performance, reliability, and security when configuring TCP settings for your application
// Consult with network and security experts as needed to ensure that your TCP settings align with best practices
// and organizational policies
// Remember that TCP is a complex protocol with many interacting components, and changes to one aspect can
// have unintended consequences on others
// Always test changes in a controlled environment before deploying them to production systems
// and monitor their impact closely after deployment
// Stay up-to-date with the latest developments in TCP and network programming to ensure that your application
// continues to perform optimally in an evolving network landscape
// Consider using higher-level libraries or frameworks that abstract away some of the complexities of TCP   
// and provide more straightforward interfaces for common tasks
// However, always be aware of the underlying TCP settings and their impact on your application's performance
// and reliability.



