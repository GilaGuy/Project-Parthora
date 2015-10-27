#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>
#include <mutex>
#include <stack>
#include <SFML\Network.hpp>
#include "../Protocol.h"
#include "../Packet.h"

class Connection
{
public:
	Connection();
	~Connection();

	void setConnectHandler(std::function<void()> onConnect);
	void setReceiveHandler(std::function<void(const Packet&)> onReceive);
	void setDisconnectHandler(std::function<void()> onDisconnect);
	bool pollPacket(Packet& packet);

	bool start(std::string serverIP, unsigned short port);
	void stop();

	void send(const Packet& p);

private:
	void receiveThread();

	sf::TcpSocket socket;
	sf::Thread clientThread;

	std::function<void()> callbackOnConnect;
	std::function<void(const Packet&)> callbackOnReceive;
	std::function<void()> callbackOnDisconnect;

	std::stack<Packet> pendingPackets;
	std::mutex mutexPendingPackets;

	bool isConnected;
};

#endif // CONNECTION_H
