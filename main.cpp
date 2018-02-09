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

string getWebSocketKey(char * buffer) {
    
    string buf = buffer;
        
    string key;
    
    for (int i = 0; i < 24; i++) {
        key = key + buf[buf.find("Sec-WebSocket-Key") + 19 + i];
    }
    
    return key;
}

string processHandshake(string sha) {
    
    char buffer[2];
    string binary_data;
    
    for (int i = 0; i < 40; i = i + 2) {
        buffer[0] = sha[i];
        buffer[1] = sha[i + 1];
        binary_data.push_back((char)strtoul(buffer, 0, 16));
    }
    
    return binary_data;
}

int main(int argc, char **argv)
{
    
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
    while (true) {
        
        // if there are clients to be processed
        if (server.thereAreClients() != false) {
            // processing every single client
            for (int i = 0; i < server.clients.size(); i++) {
                
                // if we've handshaked this client
                if (server.clients[i].pending == true)
                {
                    // contains the data come from a client.
                    char databuf[5048];
                    
                    // if this client has sent something, output it to the console, otherwise - skip processing this client.
                    int error = recv(server.clients[i].socket, databuf, sizeof(databuf), 0);
                    
                    if (error > 0) {
                        
                        unsigned char OPCODE = databuf[0] & 0b00001111;               // opcodes like 0x1, 0x2 and so on.
                        unsigned char isMasked = (databuf[1] & 0b10000000) >> 7;      // if it's 1, it's a masked frame.
                        unsigned char bodyLength = databuf[1] & 0b01111111;
                        
                        if ((int)OPCODE == 8) {
                            server.clients.erase(server.clients.begin() + i);
                            cout << server.clients.size() << endl;
                            continue;
                        }
                        
                        int maskOffset = 0;      // contains the number of the byte which the mask starts from (it depends on bodyLength)
                        
                        unsigned long long length;
                        
                        if (bodyLength <= 125) {
                            
                            length = bodyLength;
                            maskOffset = 2;
                            
                        } else if (bodyLength == 126) {
                            
                            length = (unsigned char)databuf[2];
                            length = length << 8;
                            length = length | (unsigned char)databuf[3];
                        
                            maskOffset = 4;
                            
                        } else if (bodyLength == 127) {

                            length = (unsigned char)databuf[2];
                            
                            for (int i = 0; i < 7; i++) {
                                length = length << 8;
                                length = length | (unsigned char)databuf[3 + i];
                            }
                            
                            maskOffset = 10;
                        }
                        
                        unsigned char mask_key[4] = {
                            (unsigned char)databuf[0 + maskOffset],
                            (unsigned char)databuf[1 + maskOffset],
                            (unsigned char)databuf[2 + maskOffset],
                            (unsigned char)databuf[3 + maskOffset]
                        };
                        
                        string data_received;
                        
                        for (int i = 0; i < length; i++) {
                            data_received.push_back(databuf[i + maskOffset + 4] ^ mask_key[i % 4]);
                        }
                        cout << data_received.length() << endl;
                        // if it's true, this is the first message from a client (containing its name)
                        if (server.clients[i].name == "") {
                            server.clients[i].name = data_received;
                        
                        
                        // otherwise, if the name has already been set ... 
                        } else {
                            
                            string response;                    // contains data to be sent back
                            response.push_back(0b10000001);     // it's not masked and it's a text frame
                            response.push_back(bodyLength);
                            
                            if (bodyLength <= 125) {
                                
                                for (int i = 0; i < data_received.length(); i++) {
                                    response.push_back(data_received[i]);
                                }
                                
                            } else if (bodyLength == 126) {
                                
                                response.push_back((unsigned char)databuf[2]);
                                response.push_back((unsigned char)databuf[3]);
                                
                                for (int i = 0; i < data_received.length(); i++) {
                                    response.push_back(data_received[i]);
                                }
                                
                            } else if (bodyLength == 127) {
                                
                                for (int i = 0; i < 8; i++) {
                                    response.push_back((unsigned char)databuf[i + 2]);
                                }
                                
                                for (int i = 0; i < data_received.length(); i++) {
                                    response.push_back(data_received[i]);
                                }
                                
                            }
                            
                            // sending a message to everybody except for the one who've sent the message
                            if (server.clients.size() > 1) {
                                for (int c = 0; c < server.clients.size(); c++) {
                                    if (i != c) {
                                        send(server.clients[c].socket, response.c_str(), response.length(), 0);
                                    }
                                }
                            }
                            
                            
                        }
                        
                        memset(databuf, 0, 5048);
        
                    }
                    
                    // if there hasn't been any data to be processed, continue.
                    else {
                        continue;
                    }
                }
                
                // if there hasn't been a handshake with a client
                else {
                    cout  << "handshaking" << endl;
                    
                    int error = recv(server.clients[i].socket, buf, sizeof(buf), 0);
                    
                    if (error > 0) {
                        string sha = sha1(getWebSocketKey(buf) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
                        string binary_data = processHandshake(sha);
                        string encodedData = base64_encode(reinterpret_cast<const unsigned char*>(binary_data.c_str()), binary_data.size());
                        string headers = "HTTP/1.1 101 Web Socket Protocol Handshake\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "+encodedData+"\r\n\r\n";
                        send(server.clients[i].socket, reinterpret_cast<const char*>(headers.c_str()), headers.size(), 0);
                        memset(buf, 0, 1024);
                        server.clients[i].pending = true;
                    }

                    else {
                        continue;
                    }
                }
            }
        }
        
        sf::sleep(sf::milliseconds(10));
    }
    system("pause");
    return 0;
}

