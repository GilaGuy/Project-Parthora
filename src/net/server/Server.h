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
	Server();
	~Server();

	void setNewClientHandler(std::function<void(Client*)> onNewClient);
	void setReceiveHandler(std::function<void(const Packet&, Client*)> onReceive);

	bool start(unsigned int port);
	void stop();

	client_iterator killClient(client_iterator it_c);

	bool isRunning();

private:
	void receiveThread();

	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::vector<Client*> clients;
	sf::Thread serverThread;
	std::function<void(Client*)> callbackOnNewClient;
	std::function<void(const Packet&, Client*)> callbackOnReceive;

	bool is_running, thread_running;
};

#endif // SERVER_H
