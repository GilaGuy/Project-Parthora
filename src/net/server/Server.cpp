/**
 * The server.
 *
 * @date       April 18, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      Provides an interface for operating the server.
 *             Handles all of the listening and accepting, socket multiplexing and client handling logic.
 */

#include "Server.h"
#include "../Shared.h"

#include <iostream>
#include <iomanip>

void Server::send(Packet p, Client* c)
{
	std::string toSend;
	size_t length = p.encode(toSend);

	c->socket.send(toSend.c_str(), length);

	std::cout << "SENT c=" << c->id << ", " << std::setfill('0') << std::setw(4) << length << " bytes>" << toSend << std::endl;
}

Server::Server() :
	clients(new ClientManager()),
	serverThread(&Server::receiveThread, this),
	callbackOnConnect(nullptr),
	callbackOnReceive(nullptr),
	callbackOnDisconnect(nullptr),
	is_running(false),
	thread_running(false)
{}

Server::~Server()
{
	stop();
	delete clients;
}

void Server::setConnectHandler(std::function<void(Client*)> onConnect)
{
	callbackOnConnect = onConnect;
}

void Server::setReceiveHandler(std::function<void(const Packet&, Client*)> onReceive)
{
	callbackOnReceive = onReceive;
}

void Server::setDisconnectHandler(std::function<void(Client*)> onDisconnect)
{
	callbackOnDisconnect = onDisconnect;
}

bool Server::start(unsigned short port)
{
	if (isRunning()) return false;

	if (listener.listen(port) != sf::Socket::Done) return false;
	selector.add(listener);
	serverThread.launch();

	return true;
}

void Server::stop()
{
	is_running = false;

	listener.close();

	if (thread_running) return;

	selector.clear();
	clients->clear();
}

bool Server::isRunning()
{
	return (is_running || thread_running);
}

void Server::receiveThread()
{
	is_running = true;
	thread_running = true;

	while (is_running)
	{
		if (selector.wait())
		{
			if (selector.isReady(listener)) // new connections
			{
				Client* newClient = clients->add();

				if (listener.accept(newClient->socket) == sf::Socket::Done)
				{
					selector.add(newClient->socket);

					callbackOnConnect(newClient);
				}
				else
				{
					clients->rem(newClient);
				}
			}
			else // other events (data receive / client disconnects)
			{
				ClientManager::ListIter it = clients->getList().begin(); Client* c;

				while (it != clients->getList().end())
				{
					c = *it;

					if (selector.isReady(c->socket))
					{
						char buffer[Packet::MAX_SIZE]; Packet p;

						size_t received;
						if (c->socket.receive(buffer, Packet::MAX_SIZE, received) == sf::Socket::Done)
						{
							std::cout << "RECV c=" << c->id << ", " << std::setfill('0') << std::setw(4) << received << " bytes>";

							p.decode(buffer, received);
							callbackOnReceive(p, c);

							++it;
						}
						else
						{
							callbackOnDisconnect(c);

							selector.remove(c->socket);
							it = clients->rem(it);
						}
					}
					else
					{
						++it;
					}
				}
			}
		}
	}

	std::cout << "Server receive thread stopped!" << std::endl;

	thread_running = false;

	stop();
}
