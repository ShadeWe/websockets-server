#include <winsock2.h>
#include <vector>
#include "Client.h"

class Server
{
private:

    // true when there at least one client.
    bool pendingClients = false;
    
    // is used to set an id to a client.
    int counter = 1;
    
    int socketSize;
    SOCKADDR_IN socketAddress;
    SOCKET serverSocket;
    
public:

    // all clients are placed here.
    std::vector <Client> clients;
    
    // here come getters
    SOCKET getServerSocket()             { return this->serverSocket;   }
    SOCKADDR_IN getServerSocketAddress() { return this->socketAddress;  }
    int getSocketSize()                  { return this->socketSize;     }
    bool thereAreClients()               { return this->pendingClients; }
    
    // accepts connections to the server socket, pushes clients to the vector with clients. 
    void HandlingConnection();
    
    // constructor.
    Server();

};
