#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>
#include "../Protocol.h"
#include "../Packet.h"

class Connection
{
public:
	Connection(std::function<void(const Packet&, sf::TcpSocket&)> onReceive = nullptr);
	~Connection();

	void setReceiveHandler(std::function<void(const Packet&, sf::TcpSocket&)> onReceive);

	bool start(std::string serverIP, unsigned int port);
	void stop();

	void send(const Packet& p);

private:
	void receiveThread();

	sf::TcpSocket socket;
	sf::Thread clientThread;
	std::function<void(const Packet&, sf::TcpSocket&)> callbackOnReceive;

	bool isConnected;
};

#endif // CONNECTION_H
