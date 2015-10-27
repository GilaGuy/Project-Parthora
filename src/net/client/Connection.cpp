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

#include <iostream>
#include <iomanip>

Connection::Connection() :
	isConnected(false),
	clientThread(&Connection::receiveThread, this),
	callbackOnReceive(nullptr)
{}

Connection::~Connection()
{}

void Connection::setConnectHandler(std::function<void()> onConnect)
{
	callbackOnConnect = onConnect;
}

void Connection::setReceiveHandler(std::function<void(const Packet&)> onReceive)
{
	callbackOnReceive = onReceive;
}

void Connection::setDisconnectHandler(std::function<void()> onDisconnect)
{
	callbackOnDisconnect = onDisconnect;
}

std::vector<Packet> Connection::getPendingPackets()
{
	mutexPendingPackets.lock();

	std::vector<Packet> copyOfPendingPackets = pendingPackets;
	pendingPackets.clear();

	mutexPendingPackets.unlock();

	return copyOfPendingPackets;
}

bool Connection::start(std::string serverIP, unsigned short port)
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
	std::string toSend;
	size_t length = p.encode(toSend);

	socket.send(toSend.c_str(), length);

	std::cout << "SENT " << std::setfill('0') << std::setw(4) << length << " bytes>" << toSend << std::endl;
}

void Connection::receiveThread()
{
	isConnected = true;

	if (callbackOnConnect != nullptr) callbackOnConnect();

	while (isConnected)
	{
		char buffer[Packet::MAX_SIZE]; Packet p;

		size_t received;
		if (socket.receive(buffer, Packet::MAX_SIZE, received) == sf::Socket::Done)
		{
			std::cout << "RECV " << std::setfill('0') << std::setw(4) << received << " bytes>";

			p.decode(buffer, received);

			if (callbackOnReceive == nullptr)
			{
				mutexPendingPackets.lock();

				pendingPackets.push_back(p);

				mutexPendingPackets.unlock();
			}
			else
			{
				callbackOnReceive(p);
			}
		}
		else
		{
			stop();
			break;
		}
	}

	if (callbackOnDisconnect != nullptr) callbackOnDisconnect();
}
