#include <winsock2.h>
#include <string>

#ifndef CLIENT_H
#define CLIENT_H

using namespace std;

class Client
{

public:
	string name = "";
	int id;
	bool pending = false;
	SOCKET socket;

};

#endif // CLIENT_H
