#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 5000

// can test the code with the command: echo -n "Test" | nc -u 127.0.0.1 5000

int main() {
    
    // SERVER:
    struct sockaddr_in saddr = {
        .sin_family = AF_INET, // the family for UDP/TCP
        .sin_addr.s_addr = INADDR_ANY, // any address can be used
        .sin_port = htons(PORT) // htons() changes the byte order
    };

    int option = 1;
    int saddrSize = sizeof(saddr);
    int socketServer = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM = UDP
    setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)); // setting up the socket

    if (socketServer == -1) {
        std::cerr << "Not able to create socket" << std::endl;
        return -1;
    }

    // BINDING:
    bind(socketServer, (struct sockaddr*)&saddr, sizeof(saddr));

    // PRINT:
    std::cout << "[Server] Listening on port " << PORT << std::endl;

    char buff[4096];
    int sizeByteOfReceivedData;
    struct sockaddr_in caddr; // Client address structure
    socklen_t caddrSize = sizeof(caddr);

    // WAITING:
    while (true) {
        // RECEIVE DATA:
        sizeByteOfReceivedData = recvfrom(socketServer, buff, sizeof(buff), 0, (struct sockaddr*)&caddr, &caddrSize);

        if (sizeByteOfReceivedData == -1) {
            std::cerr << "Error when receiving message" << std::endl;
            break;
        }

        // Find who is the client
        char hostClient[NI_MAXHOST];
        char portClient[NI_MAXSERV];
        memset(hostClient, 0, NI_MAXHOST);
        memset(portClient, 0, NI_MAXSERV);
        if (getnameinfo((sockaddr*)&caddr, caddrSize, hostClient, NI_MAXHOST, portClient, NI_MAXSERV, 0)) {
            std::cout << "--> " << hostClient << " connected to the port " << portClient << std::endl;
        } else {
            inet_ntop(AF_INET, &caddr.sin_addr, hostClient, NI_MAXHOST);
            std::cout << "--> " << hostClient << " connected to the port " << ntohs(caddr.sin_port) << std::endl;
        }

        // PROCESS AND RESPOND TO DATA:
        sendto(socketServer, buff, sizeByteOfReceivedData, 0, (struct sockaddr*)&caddr, caddrSize);

        std::cout << std::string(buff, 0, sizeByteOfReceivedData) << std::endl;
    }

    close(socketServer);

    return 0;
}
