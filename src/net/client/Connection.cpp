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

Connection::Connection(std::function<void(const Packet&, sf::TcpSocket&)> onReceive) :
isConnected(false),
clientThread(&Connection::receiveThread, this),
callbackOnReceive(onReceive)
{}

Connection::~Connection()
{}

void Connection::setReceiveHandler(std::function<void(const Packet&, sf::TcpSocket&)> onReceive)
{
	callbackOnReceive = onReceive;
}

bool Connection::start(std::string serverIP, unsigned int port)
{
	if (isConnected) return false;

	if (socket.connect(serverIP, port) != sf::Socket::Done) return false;

	return true;
}

void Connection::stop()
{
	socket.disconnect();

	isConnected = false;
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
			callbackOnReceive(p, socket);
		}
		else
		{
			stop();
			break;
		}
	}
}
