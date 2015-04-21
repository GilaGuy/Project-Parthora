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
 * @notes
 */

#include "Server.h"
#include "../Protocol.h"

Server::Server() :
isRunning(false),
serverThread(&Server::threadServer, this)
{}

Server::~Server()
{}

void Server::start(unsigned int port)
{
	if (isRunning) return;

	listener.listen(port);
	selector.add(listener);
	serverThread.launch();
}

void Server::stop()
{
	selector.clear();

	for (Client* c : clients)
	{
		c->socket.disconnect();
		delete c;
	}
	clients.clear();

	listener.close();

	isRunning = false;
}

void Server::threadServer()
{
	isRunning = true;

	while (isRunning)
	{
		if (selector.wait())
		{
			if (selector.isReady(listener))
			{
				Client* client = new Client();
				if (listener.accept(client->socket) == sf::Socket::Done)
				{
					clients.push_back(client);
					selector.add(client->socket);
				}
				else
				{
					delete client;
				}
			}
			else
			{
				for (Client* c : clients)
				{
					if (selector.isReady(c->socket))
					{
						sf::Packet packet;
						if (c->socket.receive(packet) == sf::Socket::Done)
						{

						}
					}
				}
			}
		}
		else
		{
			stop();
			break;
		}
	}
}
