#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

class Client;

class Server
{
public:
	Server();
	~Server();

	void start(unsigned int port);
	void stop();

private:
	void threadServer();

	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::vector<Client*> clients;
	sf::Thread serverThread;

	bool isRunning;
};

#endif // SERVER_H
