#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include "../Packet.h"
#include "../entities/Client.h"

struct Client;
class ClientManager;

class Server
{
public:
	static void send(Packet p, Client* c);

	Server();
	~Server();

	void setConnectHandler(std::function<void(Client*)> onConnect);
	void setReceiveHandler(std::function<void(const Packet&, Client*)> onReceive);
	void setDisconnectHandler(std::function<void(Client*)> onDisconnect);

	bool start(unsigned short port);
	void stop();

	inline ClientManager& getClientManager() { return *clients; }
	inline ClientManager::List& getClients() { return clients->getList(); }

	bool isRunning();

private:
	void receiveThread();

	sf::TcpListener listener;
	sf::SocketSelector selector;
	ClientManager* clients;
	sf::Thread serverThread;
	std::function<void(Client*)> callbackOnConnect;
	std::function<void(const Packet&, Client*)> callbackOnReceive;
	std::function<void(Client*)> callbackOnDisconnect;

	bool is_running, thread_running;
};

#endif // SERVER_H
