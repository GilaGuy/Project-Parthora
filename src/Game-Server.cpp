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

int getScreenIdx(Client::ID ownerID)
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
	cout << "Client " << c->socket.getRemoteAddress() << " [" << c << "]" << " connected!" << endl;

	c->id = ++clientID;
	c->screenIdx = screens.size();
	screens.push_back({ ++screenID, c });
}

void onReceive(const Packet& p, Client* c)
{
	switch (p.mType)
	{
	case MessageType::CLIENT_INFO:
		c->params.name = p.get(0);
		c->params.pp.colorBegin = sf::Color(p.get<sf::Uint16>(1), p.get<sf::Uint16>(2), p.get<sf::Uint16>(3), p.get<sf::Uint16>(4));
		c->params.pp.colorEnd = sf::Color(p.get<sf::Uint16>(5), p.get<sf::Uint16>(6), p.get<sf::Uint16>(7), p.get<sf::Uint16>(8));
		break;

	case MessageType::CROSS_SCREENS:
	{
		Client::ID clientID = p.get<Client::ID>(0); // the client that's crossing screens
		CrossingDirection crossDir = static_cast<CrossingDirection>(p.get<int>(2));
		int clientScreen = clientID == Client::ID_MYSELF ? c->screenIdx : getScreenIdx(clientID);
		int targetScreen = clientScreen;

		if (clientScreen == -1) return;

		switch (crossDir)
		{
		case LEFT:
			--targetScreen;
			break;
		case RIGHT:
			++targetScreen;
			break;
		default:
			break;
		}

		if (targetScreen < 0 || targetScreen == screens.size()) return;

		// add more params from the client that's crossing...

		Packet pCS = p;

		Client* crossingClient = screens.at(clientScreen).owner;

		pCS.add(crossingClient->params.pp.colorBegin.r);
		pCS.add(crossingClient->params.pp.colorBegin.g);
		pCS.add(crossingClient->params.pp.colorBegin.b);
		pCS.add(crossingClient->params.pp.colorBegin.a);

		pCS.add(crossingClient->params.pp.colorEnd.r);
		pCS.add(crossingClient->params.pp.colorEnd.g);
		pCS.add(crossingClient->params.pp.colorEnd.b);
		pCS.add(crossingClient->params.pp.colorEnd.a);

		cout << pCS.encode() << endl;

		// send a CROSS_SCREENS msg to the owner of the target screen
		Server::send(pCS, screens.at(targetScreen).owner);
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
	cout << "Client " << c->socket.getRemoteAddress() << " [" << c << "]" << " disconnected!" << endl;

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
