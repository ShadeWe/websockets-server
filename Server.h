#include <winsock2.h>
#include <vector>
#include "Client.h"

class Server
{
private:
    int socketSize;
    SOCKADDR_IN socketAddress;
    SOCKET serverSocket;
    
public:
    std::vector <Client> clients;
    SOCKET getServerSocket() {
        return this->serverSocket;
    }
    
    SOCKADDR_IN getServerSocketAddress() {
        return this->socketAddress;
    }
    
    int getSocketSize() {
        return this->socketSize;
    }
    
    void HandlingConnection();

    Server();

};
