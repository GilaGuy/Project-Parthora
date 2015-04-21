#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>
#include "../Protocol.h"
#include "../Packet.h"

class Connection
{
public:
	Connection(std::function<void(Packet)> onReceive);
	~Connection();

	bool start(std::string serverIP, unsigned int port);
	void stop();

private:
	void receiveThread();

	sf::TcpSocket socket;
	sf::Thread clientThread;
	std::function<void(Packet)> callbackOnReceive;

	bool isConnected;
};

#endif // CONNECTION_H
