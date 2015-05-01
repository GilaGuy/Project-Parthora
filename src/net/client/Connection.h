#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>
#include <mutex>
#include "../Protocol.h"
#include "../Packet.h"

class Connection
{
public:
	Connection();
	~Connection();

	void setReceiveHandler(std::function<void(const Packet&)> onReceive);
	std::vector<Packet> getPendingPackets();

	bool start(std::string serverIP, unsigned int port);
	void stop();

	void send(const Packet& p);

private:
	void receiveThread();

	sf::TcpSocket socket;
	sf::Thread clientThread;

	std::function<void(const Packet&)> callbackOnReceive;
	std::vector<Packet> pendingPackets;
	std::mutex mutex_pendingPackets;

	bool isConnected;
};

#endif // CONNECTION_H
