#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include "../Packet.h"

struct Client;

class Server
{
public:
	Server(std::function<void(Packet)> onReceive);
	~Server();

	bool start(unsigned int port);
	void stop();

private:
	void receiveThread();

	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::vector<Client*> clients;
	sf::Thread serverThread;
	std::function<void(Packet)> callbackOnReceive;

	bool isRunning;
};

#endif // SERVER_H
