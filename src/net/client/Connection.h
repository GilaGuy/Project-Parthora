#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>
#include "../Protocol.h"
#include "../Packet.h"

class Connection
{
public:
	Connection();
	~Connection();

	void setReceiveHandler(std::function<void(const Packet&)> onReceive);

	bool start(std::string serverIP, unsigned int port);
	void stop();

	void send(const Packet& p);

	std::vector<Packet> pendingPackets;

private:
	void receiveThread();

	sf::TcpSocket socket;
	sf::Thread clientThread;
	std::function<void(const Packet&)> callbackOnReceive;

	bool isConnected;
};

#endif // CONNECTION_H
