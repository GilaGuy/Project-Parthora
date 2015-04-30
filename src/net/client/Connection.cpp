/**
* The client's connection.
*
* @date       April 21, 2015
*
* @revisions
*
* @designer   Melvin Loho
*
* @programmer Melvin Loho
*
* @notes      Provides an interface for the client to connect to the server and exchange data.
*/

#include "Connection.h"

Connection::Connection() :
isConnected(false),
clientThread(&Connection::receiveThread, this),
callbackOnReceive(nullptr)
{}

Connection::~Connection()
{}

void Connection::setReceiveHandler(std::function<void(const Packet&)> onReceive)
{
	callbackOnReceive = onReceive;
}

bool Connection::start(std::string serverIP, unsigned int port)
{
	if (isConnected) return false;

	if (socket.connect(serverIP, port) != sf::Socket::Done) return false;

	clientThread.launch();

	return true;
}

void Connection::stop()
{
	socket.disconnect();

	isConnected = false;
}

void Connection::send(const Packet& p)
{
	socket.send(p.encode().c_str(), PACKET_SIZE);
}

void Connection::receiveThread()
{
	isConnected = true;

	while (isConnected)
	{
		char buffer[PACKET_SIZE]; Packet p;

		size_t received;
		if (socket.receive(buffer, PACKET_SIZE, received) == sf::Socket::Done)
		{
			p.decode(buffer);
			callbackOnReceive(p);
		}
		else
		{
			stop();
			break;
		}
	}
}
