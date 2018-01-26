#include <winsock2.h>

#ifndef CLIENT_H
#define CLIENT_H

class Client
{
public:
    
    bool pending = false;
    SOCKET socket;
    

    Client();
    ~Client();

};

#endif // CLIENT_H
