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

	cerr << "Client screen not found! Owner ID: " << ownerID << endl;

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
	cout << "recv> " << p.encode() << endl;

	switch (p.mType)
	{
	case CLIENT_INFO:
		c->params.name = p.get(0);
		c->params.pp.colorBegin = sf::Color(p.get<sf::Uint16>(1), p.get<sf::Uint16>(2), p.get<sf::Uint16>(3), p.get<sf::Uint16>(4));
		c->params.pp.colorEnd = sf::Color(p.get<sf::Uint16>(5), p.get<sf::Uint16>(6), p.get<sf::Uint16>(7), p.get<sf::Uint16>(8));
		break;

	case CROSS_SCREENS:
	{
		Client::ID clientID = p.get<Client::ID>(0); // the client that's crossing screens
		CrossingDirection crossDir = static_cast<CrossingDirection>(p.get<int>(2));
		int clientScreen = clientID == Client::ID_MYSELF ? c->screenIdx : getScreenIdx(clientID);
		int targetScreen = clientScreen;

		switch (crossDir)
		{
		case LEFT:
			--targetScreen;
			break;
		case RIGHT:
			++targetScreen;
			break;
		}

		// check if screen exists and send back the status to the client

		Packet pCrossingStatus;
		pCrossingStatus.mType = CROSS_STATUS;

		if ((clientScreen == -1)
			||
			(targetScreen < 0 || targetScreen == screens.size()))
		{
			pCrossingStatus.add(clientID);
			pCrossingStatus.add(false);
			Server::send(pCrossingStatus, c); // < not working...
			return;
		}
		else
		{
			pCrossingStatus.add(clientID);
			pCrossingStatus.add(true);
			Server::send(pCrossingStatus, c);
		}

		// add the new screen to the client's list of external screens

		screens.at(clientScreen).owner->externalScreenOccupancies.push_back(&screens.at(targetScreen));

		// relay the packet to the target client with the new info

		Packet pCrossScreens = p;

		pCrossScreens.mParams.at(0) = to_string(screens.at(clientScreen).owner->id);

		Client* crossingClient = screens.at(clientScreen).owner;

		pCrossScreens.add(crossingClient->params.pp.colorBegin.r);
		pCrossScreens.add(crossingClient->params.pp.colorBegin.g);
		pCrossScreens.add(crossingClient->params.pp.colorBegin.b);
		pCrossScreens.add(crossingClient->params.pp.colorBegin.a);

		pCrossScreens.add(crossingClient->params.pp.colorEnd.r);
		pCrossScreens.add(crossingClient->params.pp.colorEnd.g);
		pCrossScreens.add(crossingClient->params.pp.colorEnd.b);
		pCrossScreens.add(crossingClient->params.pp.colorEnd.a);

		// send a CROSS_SCREENS msg to the owner of the target screen
		Server::send(pCrossScreens, screens.at(targetScreen).owner);
	}
	break;

	case UPDATE_POS:
		cout << "UPDATE POS" << endl;
		break;

	case REMOVE_TRACKING:
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

				p.mType = CLIENT_DISCONNECTED;
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
