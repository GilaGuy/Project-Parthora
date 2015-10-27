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
#include "../Protocol.h"
#include "../entities/Client.h"

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
	is_running(false),
	thread_running(false),
	serverThread(&Server::receiveThread, this),
	callbackOnConnect(nullptr),
	callbackOnReceive(nullptr),
	callbackOnDisconnect(nullptr)
{}

Server::~Server()
{
	stop();
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

	for (ClientListIter it = clients.begin(); it != clients.end();)
	{
		it = killClient(it);
	}

	selector.clear();
	clients.clear();
}

Server::ClientListIter Server::killClient(ClientListIter it_c)
{
	Client* c = (*it_c);

	selector.remove(c->socket);
	c->socket.disconnect();
	delete c; c = nullptr;

	return clients.erase(it_c);
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
				Client* client = new Client();
				if (listener.accept(client->socket) == sf::Socket::Done)
				{
					clients.push_back(client);
					selector.add(client->socket);

					callbackOnConnect(client);
				}
				else
				{
					delete client;
				}
			}
			else // other events (data receive / client disconnects)
			{
				ClientListIter it_c = clients.begin(); Client* c;

				while (it_c != clients.end())
				{
					c = (*it_c);

					if (selector.isReady(c->socket))
					{
						char buffer[Packet::MAX_SIZE]; Packet p;

						size_t received;
						if (c->socket.receive(buffer, Packet::MAX_SIZE, received) == sf::Socket::Done)
						{
							std::cout << "RECV c=" << c->id << ", " << std::setfill('0') << std::setw(4) << received << " bytes>";

							p.decode(buffer, received);
							callbackOnReceive(p, c);

							++it_c;
						}
						else
						{
							callbackOnDisconnect(c);

							it_c = killClient(it_c);
						}
					}
					else
					{
						++it_c;
					}
				}
			}
		}
	}

	std::cout << "Server receive thread stopped!" << std::endl;

	thread_running = false;

	stop();
}
