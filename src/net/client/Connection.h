#ifndef CONNECTION_H
#define CONNECTION_H

#include <mutex>
#include <stack>
#include <SFML/Network.hpp>
#include "../Packet.h"

class Connection
{
public:
	class Event
	{
	public:
		// EVENT TYPES>

		enum EventType
		{
			CONNECT,
			PACKET,
			DISCONNECT
		};

		// DATA>

		// the type of the event
		EventType type;

		// the possible contents of the event>

		Packet packet;
	};

	Connection();
	~Connection();

	bool start(std::string serverIP, unsigned short port);
	void stop();

	bool pollEvent(Event& connEvent);

	void send(const Packet& p);

	bool isConnected();

private:
	void receiveThread();

	sf::TcpSocket socket;
	sf::Thread clientThread;

	std::stack<Event> connEvents;
	std::mutex mutexConnEvents;

	bool is_connected;
};

#endif // CONNECTION_H
