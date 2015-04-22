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

Server::Server(std::function<void(Packet, Client*)> onReceive) :
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

void Server::killClient(client_iterator& it)
{
	Client* c = (*it);

	selector.remove(c->socket);
	c->socket.disconnect();
	it = clients.erase(it);
	delete c; c = nullptr;
}

void Server::receiveThread()
{
	isRunning = true;

	while (isRunning)
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
				}
				else
				{
					delete client;
				}
			}
			else // other events (data receive / client disconnects)
			{
				client_iterator it = clients.begin(); Client* c;

				while (it != clients.end())
				{
					c = (*it);

					if (selector.isReady(c->socket))
					{
						char buffer[PACKET_SIZE]; Packet p;

						size_t received;
						if (c->socket.receive(buffer, PACKET_SIZE, received) == sf::Socket::Done)
						{
							p.decode(buffer);
							callbackOnReceive(p, c);

							++it;
						}
						else
						{
							killClient(it);
						}
					}
					else
					{
						++it;
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
