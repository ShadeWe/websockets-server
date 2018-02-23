
#include <winsock2.h>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include "Server.h"

using namespace std;

Server::Server()
{

	unsigned long nb = 1;

	this->socketSize = sizeof(this->socketAddress);

	this->socketAddress.sin_family = AF_INET;
	this->socketAddress.sin_port = htons(1234);
	this->socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (this->serverSocket == INVALID_SOCKET) {
		cout << "something went wrong while creating a socket :(" << endl << endl;
	}


	int err = ioctlsocket(this->serverSocket, FIONBIO, (unsigned long *)&nb);
	if (err == SOCKET_ERROR) {
		cout << "an error occured" << endl;
	}

	// binding the address to our socket with a port, inet_addr, protocol set.
	bind(this->serverSocket, (SOCKADDR*)&this->socketAddress, this->socketSize);


}

void Server::messageBack() {

}


void Server::HandlingConnection() {

	while (true) {

		SOCKET socket = accept(this->serverSocket, (SOCKADDR*)&this->socketAddress, &this->socketSize);

		// there has been a connection
		if (socket != INVALID_SOCKET) {

			printf("new client\n");

			// creating an object for our client
			Client client;
			client.pending = false;
			client.socket = socket;
			client.id = this->counter;

			this->counter++;

			// add our new client to the vector of clients
			this->clients.push_back(client);

			// if there's at least one client, set true to the pendingClients.
			if (this->clients.size() != 0) {
				this->pendingClients = true;
			}
			else {
				this->pendingClients = false;
			}


		}
		sf::sleep(sf::milliseconds(10));
	}
}

