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

Server::Server(std::function<void(const Packet&, Client*)> onReceive) :
is_running(false),
thread_running(false),
serverThread(&Server::receiveThread, this),
callbackOnReceive(onReceive)
{}

Server::~Server()
{}

void Server::setReceiveHandler(std::function<void(const Packet&, Client*)> onReceive)
{
	callbackOnReceive = onReceive;
}

bool Server::start(unsigned int port)
{
	if (isRunning()) return false;

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

	is_running = false;
}

void Server::killClient(Client* c)
{
	selector.remove(c->socket);
	c->socket.disconnect();
	clients.erase(std::remove(clients.begin(), clients.end(), c), clients.end());
	delete c; c = nullptr;
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
							killClient(c);
							++it;
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

	thread_running = false;
}
