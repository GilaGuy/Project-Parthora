/**
 * Project Parthora's server driver.
 *
 * @date       April 23, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      Runs and manages the server.
 *             Handles the data exchanging logic of the server.
 */

#include <map>
#include "net\server\Server.h"
#include "net\Protocol.h"

#include <iostream>

using namespace std;

Client::ID clientID = 0;
Screen::ID screenID = 0;

std::vector<Screen> screens;
typedef std::vector<Screen>::iterator screen_iterator;

int getScreen(Client::ID ownerID)
{
	for (int s = 0; s < screens.size(); ++s)
	{
		if (screens[s].owner->id == ownerID)
		{
			return s;
		}
	}

	return -1;
}

void onConnect(Client* c)
{
	std::cout << "Client " << c->socket.getRemoteAddress() << " [" << c << "]" << " connected!" << std::endl;

	c->id = ++clientID;
	screens.push_back({ ++screenID, c });
}

void onReceive(const Packet& p, Client* c)
{
	switch (p.mType)
	{
	case MessageType::UPDATE_INFO:
		c->params.name = p.get(0);
		c->params.pp.colorBegin = sf::Color(p.get<sf::Uint16>(1), p.get<sf::Uint16>(2), p.get<sf::Uint16>(3), p.get<sf::Uint16>(4));
		c->params.pp.colorEnd = sf::Color(p.get<sf::Uint16>(5), p.get<sf::Uint16>(6), p.get<sf::Uint16>(7), p.get<sf::Uint16>(8));
		c->params.pp.x = p.get<float>(9);
		c->params.pp.y = p.get<float>(10);
		break;

	case MessageType::CROSS_SCREENS:
	{
		CrossingDirection crossDir = static_cast<CrossingDirection>(p.get<int>(0));
		Client::ID clientID = p.get<Client::ID>(1); // the client that's crossing screens
		int targetScreen = clientID == 0 ? getScreen(c->id) : getScreen(clientID);

		if (targetScreen == -1) return;

		if (crossDir == LEFT) targetScreen = --targetScreen;
		else if (crossDir == RIGHT) targetScreen = ++targetScreen;

		if (targetScreen < 0 || targetScreen == screens.size()) return;

		// send the cross_screens msg to the owner of the target screen
	}
	break;

	case MessageType::UPDATE_POS:
		cout << "UPDATE POS" << endl;
		break;

	case MessageType::REMOVE_TRACKING:
		break;

	default:
		cout << "WUT" << endl;
	}
}

void onDisconnect(Client* c)
{
	std::cout << "Client " << c->socket.getRemoteAddress() << " [" << c << "]" << " disconnected!" << std::endl;

	for (screen_iterator it = screens.begin(); it != screens.end();)
	{
		if (it->owner == c)
		{
			if (!c->externalScreenOccupancies.empty())
			{
				Packet p;

				p.mType = MessageType::CLIENT_DISCONNECTED;
				p.add(c->id);

				for (Screen* s : c->externalScreenOccupancies)
				{
					Server::send(p, s->owner);
				}
			}

			it = screens.erase(it);
			return;
		}
		else
		{
			++it;
		}
	}
}

int main()
{
	Server server;

	server.setConnectHandler(onConnect);
	server.setReceiveHandler(onReceive);
	server.setDisconnectHandler(onDisconnect);

	server.start(12345);

	cout << "Server running..." << endl;
	cout << "Enter k to kill the server" << endl;

	while (getchar() != 'k');

	cout << "Server stopping..." << endl;

	server.stop();

	while (server.isRunning());

	cout << "Server stopped!" << endl;

	return 0;
}
