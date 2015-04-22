#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include "../Packet.h"

struct Client;

class Server
{
	typedef std::vector<Client*>::iterator client_iterator;

public:
	Server(std::function<void(Packet, Client*)> onReceive);
	~Server();

	bool start(unsigned int port);
	void stop();

	void killClient(Client* c);

private:
	void receiveThread();

	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::vector<Client*> clients;
	sf::Thread serverThread;
	std::function<void(Packet, Client*)> callbackOnReceive;

	bool isRunning;
};

#endif // SERVER_H
