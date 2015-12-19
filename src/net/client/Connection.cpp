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
	is_connected(false),
	clientThread(&Connection::receiveThread, this)
{}

Connection::~Connection()
{}

bool Connection::start(std::string serverIP, unsigned short port)
{
	if (is_connected) return false;

	if (socket.connect(serverIP, port) != sf::Socket::Done) return false;

	clientThread.launch();

	return true;
}

void Connection::stop()
{
	socket.disconnect();

	is_connected = false;
}

bool Connection::pollEvent(Event& connEvent)
{
	bool available;

	mutexConnEvents.lock();

	available = !connEvents.empty();
	if (available)
	{
		connEvent = connEvents.top();
		connEvents.pop();
	}

	mutexConnEvents.unlock();

	return available;
}

void Connection::send(const Packet& p)
{
	std::string toSend;
	size_t length = p.encode(toSend);

	socket.send(toSend.c_str(), length);

	std::cout << "SENT " << std::setfill('0') << std::setw(4) << length << " bytes>" << toSend << std::endl;
}

bool Connection::isConnected()
{
	return is_connected;
}

void Connection::receiveThread()
{
	is_connected = true;

	mutexConnEvents.lock();

	connEvents.push(Event());
	connEvents.top().type = Event::CONNECT;

	mutexConnEvents.unlock();

	while (is_connected)
	{
		char buffer[Packet::MAX_SIZE];

		size_t received;
		if (socket.receive(buffer, Packet::MAX_SIZE, received) == sf::Socket::Done)
		{
			std::cout << "RECV " << std::setfill('0') << std::setw(4) << received << " bytes>";

			Packet packet;

			packet.decode(buffer, received);

			mutexConnEvents.lock();

			connEvents.push(Event());
			connEvents.top().type = Event::PACKET;
			connEvents.top().packet = packet;

			mutexConnEvents.unlock();
		}
		else
		{
			stop();
			break;
		}
	}

	mutexConnEvents.lock();

	connEvents.push(Event());
	connEvents.top().type = Event::DISCONNECT;

	mutexConnEvents.unlock();
}
