#include <winsock2.h>
#include "base64.h"
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "sha.h"
#include "Client.h"
#include "Server.h"

using namespace std;

void HandlingConnection(SOCKET & socket) {
    while (true) {
        
    }
}

string getWebSocketKey(char * buffer) {
    
    string buf = buffer;
        
    string key;
    
    for (int i = 0; i < 24; i++) {
        key = key + buf[buf.find("Sec-WebSocket-Key") + 19 + i];
    }
    
    return key;
}

string processHandshake(string & binary_data, string sha) {
    char test[2];
    
    for (int i = 0; i < 40; i = i + 2) {
        test[0] = sha[i];
        test[1] = sha[i + 1];
        binary_data.push_back((char)strtoul(test, 0, 16));
    }
    
}

int main(int argc, char **argv)
{
    
    setlocale( LC_ALL,"Russian" );
    int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
    
    WSADATA ws;
    
    if (FAILED(WSAStartup (MAKEWORD( 1, 1 ), &ws))) {
        cout << "something went wrong :(" << endl << endl;
        return 0;
    }
    
    
    Server server;
    
    listen(server.getServerSocket(), SOMAXCONN);
    
    SOCKADDR_IN address = server.getServerSocketAddress();
    SOCKET serverSocket = server.getServerSocket();
    int socketSize = server.getSocketSize();
    
    
    sf::Thread thread(&server.HandlingConnection, &server);
    thread.launch();
    
    char buf[1024];
    char databuf[512];
    
    while (true) {
        if (server.clients.size() != 0) {
            for (int i = 0; i < server.clients.size(); i++) {
                    if (server.clients[i].pending == true)
                    {
                        if (recv(server.clients[i].socket, databuf, sizeof(databuf), 0) > 0) {
                            
                            char first_byte = databuf[0];
                            char second_byte = databuf[1];
                            char body_length = second_byte & 0b01111111;
                            
                            char mask_key[4] = {
                                databuf[2],
                                databuf[3],
                                databuf[4],
                                databuf[5]
                            };
                            
                            char data_received[128];
                            
                            for (int i = 0; i < (int)body_length; i++) {
                                data_received[i] = databuf[6 + i];
                            }
                            
                            char data_decoded[(int)body_length];
                            
                            for (int i = 0; i < (int)body_length; i++) {
                                data_decoded[i] = data_received[i] ^ mask_key[i % 4];
                            }
                            
                            cout << "data received: " << data_decoded << endl;
                        } else {
                            continue;
                        }
                    }
                    else 
                    {
                        cout  << "handshaking" << endl;
                        
                        if (recv(server.clients[i].socket, buf, sizeof(buf), 0)) {
                            string sha = sha1(getWebSocketKey(buf) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
            
                            string binary_data;
                            processHandshake(binary_data, sha);
                            
                            string encodedData = base64_encode(reinterpret_cast<const unsigned char*>(binary_data.c_str()), binary_data.size());
                            
                            string headers = "HTTP/1.1 101 Web Socket Protocol Handshake\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "+encodedData+"\r\n\r\n";
                            send(server.clients[i].socket, reinterpret_cast<const char*>(headers.c_str()), headers.size(), 0);
                            
                            for (int i=0; i<1024; i++) buf[i] = 0;
                            
                            server.clients[i].pending = true;
                        }
                        else {
                            continue;
                        }
                        
                        
                    }
                
                
            }
            
            
        }
    }
    
    /*
    
    if (newConnection == 0 ) {
        cout << "error occured";
    }
    else {
        cout << "someone has connected to the server, handshaking..." << endl;
        recv(newConnection, buf, sizeof(buf), 0);
        
        string sha = sha1(getWebSocketKey(buf) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        
        string binary_data;
        processHandshake(binary_data, sha);
        
        string encodedData = base64_encode(reinterpret_cast<const unsigned char*>(binary_data.c_str()), binary_data.size());
        
        string headers = "HTTP/1.1 101 Web Socket Protocol Handshake\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "+encodedData+"\r\n\r\n";
        send(newConnection, reinterpret_cast<const char*>(headers.c_str()), headers.size(), 0);
        
        while (true) {
            if (recv(newConnection, databuf, sizeof(databuf), 0)) {
                
                char first_byte = databuf[0];
                char second_byte = databuf[1];
                char body_length = second_byte & 0b01111111;
                
                char mask_key[4] = {
                    databuf[2],
                    databuf[3],
                    databuf[4],
                    databuf[5]
                };
                
                char data_received[128];
                
                for (int i = 0; i < (int)body_length; i++) {
                    data_received[i] = databuf[6 + i];
                }
                
                char data_decoded[(int)body_length];
                for (int i = 0; i < (int)body_length; i++) {
                    data_decoded[i] = data_received[i] ^ mask_key[i % 4];
                }
            }
        }
    }
    */
    system("pause");
    return 0;
}

