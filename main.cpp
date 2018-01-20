#include <winsock2.h>
#include "base64.h"
#include <iostream>
#include <string>
#include <vector>
#include "sha.h"

using namespace std;

string getWebSocketKey(char * buffer) {
    string buf = buffer;
        
    string key;
    
    for (int i = 0; i < 43; i++) {
        key = key + buf[buf.find("Sec-WebSocket-Key") + 19 + i];
    }
    
    return key;
}


int main(int argc, char **argv)
{
    int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
    
    WSADATA ws;
    
    if (FAILED(WSAStartup (MAKEWORD( 1, 1 ), &ws))) {
        cout << "something went wrong :(" << endl << endl;
        return 0;
    }
    
    
    // for holding the address of a socket
    SOCKADDR_IN socketAddress;
    
    int socketSize = sizeof(socketAddress);
    
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(1234);
    socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    
    if (s == INVALID_SOCKET) {
        cout << "something went wrong while creating a socket :(" << endl << endl;
        return 0;
    }
    
    bind(s, (SOCKADDR*)&socketAddress, socketSize);
    
    listen(s, SOMAXCONN);
    
    SOCKET newConnection = accept(s, (SOCKADDR*)&socketAddress, &socketSize);
    
    char buf[1024];

    if (newConnection == 0 ) {
        cout << "error occured";
    }
    else {
        cout << "someone has connected to the server, handshaking..." << endl;
        recv(newConnection, buf, sizeof(buf), 0);
        
        string key = getWebSocketKey(buf);
        string sha = sha1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        string encodedData = base64_encode(reinterpret_cast<const unsigned char*>(sha.c_str()), sha.size());
        
        string headers = "HTTP/1.1 101 Web Socket Protocol Handshake\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "+encodedData+"\r\n\r\n";
        
        cout << headers << endl;
        send(newConnection, reinterpret_cast<const char*>(headers.c_str()), headers.size(), 0);
        
    }
    
    system("pause");
    return 0;
}

