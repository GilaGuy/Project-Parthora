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

Server::Server(std::function<void(Packet)> onReceive) :
isRunning(false),
serverThread(&Server::receiveThread, this),
callbackOnReceive(onReceive)
{}

Server::~Server()
{}

bool Server::start(unsigned int port)
{
	if (isRunning) return false;

	if (listener.listen(port) != sf::Socket::Done) return false;
	selector.add(listener);
	serverThread.launch();

	return true;
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

void Server::receiveThread()
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
						char buffer[PACKET_SIZE]; Packet p;

						size_t received;
						if (c->socket.receive(buffer, PACKET_SIZE, received) == sf::Socket::Done)
						{
							p.decode(buffer);
							callbackOnReceive(p);
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
