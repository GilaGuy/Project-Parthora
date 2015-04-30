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

std::vector<Screen*> screens;
typedef std::vector<Screen*>::iterator screen_iterator;

int getScreenIdx(Client::ID ownerID)
{
	for (int s = 0; s < screens.size(); ++s)
	{
		if (screens[s]->owner->id == ownerID)
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

	screens.push_back(new Screen());

	Screen* newScreen = screens.back();
	newScreen->id = ++screenID;
	newScreen->owner = c;

	c->id = ++clientID;
	c->screenOwned = screens.back();
	c->screenCurrent = c->screenOwned;
}

void onReceive(const Packet& p, Client* c)
{
	cout << "recv> " << p.encode() << endl;

	switch (p.mType)
	{
	case PLAYER_INFO:
		c->params.name = p.get(0);
		c->screenOwned->size.x = p.get<unsigned int>(1);
		c->screenOwned->size.y = p.get<unsigned int>(2);
		c->params.ps.emitterPos.x = p.get<float>(3);
		c->params.ps.emitterPos.y = p.get<float>(4);
		c->screenOwned->boundaryLeft = c->screenOwned->size.x * 0.125f;
		c->screenOwned->boundaryRight = c->screenOwned->size.x - c->screenOwned->boundaryLeft;
		c->params.ps.colorBegin = sf::Color(p.get<sf::Uint16>(5), p.get<sf::Uint16>(6), p.get<sf::Uint16>(7), p.get<sf::Uint16>(8));
		c->params.ps.colorEnd = sf::Color(p.get<sf::Uint16>(9), p.get<sf::Uint16>(10), p.get<sf::Uint16>(11), p.get<sf::Uint16>(12));
		break;

	case UPDATE_POS:
	{
		int clientScreenIdx = getScreenIdx(c->id);
		int targetScreenIdx = clientScreenIdx;
		Client* client = screens.at(clientScreenIdx)->owner; // the client that's crossing screens

		if (clientScreenIdx == -1)
		{
			cerr << "UPDATE_POS: Packet sender's screen not found!!" << endl;
		}

		client->params.ps.emitterPos.x += p.get<float>(1);

		Cross cross;
		float xOffset = 0;

		if (client->params.ps.emitterPos.x < client->screenCurrent->boundaryLeft)
		{
			cross = CROSS_LEFT;
			xOffset = 0 + client->params.ps.emitterPos.x;
			--targetScreenIdx;
		}
		else if (client->params.ps.emitterPos.x > client->screenCurrent->boundaryRight)
		{
			cross = CROSS_RIGHT;
			xOffset = client->screenCurrent->size.x - client->params.ps.emitterPos.x;
			++targetScreenIdx;
		}
		else
		{
			cross = NO_CROSS;
		}

		if (cross == NO_CROSS) // within boundaries
		{
			if (!client->externalScreenOccupancies.empty())
			{
				Packet pDeletePlayer;

				pDeletePlayer.mType = DELETE_PLAYER;
				pDeletePlayer.add(client->id);

				if (client->externalScreenOccupancies.size() > 1)
				{
					for (Screen* s : client->externalScreenOccupancies)
					{
						if (s != client->screenCurrent) Server::send(pDeletePlayer, s->owner);
					}
					client->externalScreenOccupancies.clear();
					client->externalScreenOccupancies.insert(client->screenCurrent);
				}
				else
				{
					if (client->externalScreenOccupancies.find(client->screenCurrent)
						== client->externalScreenOccupancies.end())
					{
						cerr << "UHOH, the client's only external screen occupancy is not their current screen!!" << endl;
					}
				}
			}
		}
		else // beyond boundaries
		{
			if (targetScreenIdx >= 0 && targetScreenIdx < screens.size())
			{
				Screen* targetScreen = screens.at(targetScreenIdx);

				if (std::find(client->externalScreenOccupancies.begin(),
					client->externalScreenOccupancies.end(),
					targetScreen)
					== client->externalScreenOccupancies.end())
				{
					Packet pNewPlayer;
					pNewPlayer.mType = NEW_PLAYER;

					pNewPlayer.add(targetScreen == client->screenOwned ? Client::ID_MYSELF : client->id);
					pNewPlayer.add((int)cross);
					pNewPlayer.add(xOffset);
					pNewPlayer.add(c->params.ps.emitterPos.y / c->screenCurrent->size.y);

					pNewPlayer.add(client->params.name);

					pNewPlayer.add(client->params.ps.colorBegin.r);
					pNewPlayer.add(client->params.ps.colorBegin.g);
					pNewPlayer.add(client->params.ps.colorBegin.b);
					pNewPlayer.add(client->params.ps.colorBegin.a);

					pNewPlayer.add(client->params.ps.colorEnd.r);
					pNewPlayer.add(client->params.ps.colorEnd.g);
					pNewPlayer.add(client->params.ps.colorEnd.b);
					pNewPlayer.add(client->params.ps.colorEnd.a);

					Server::send(pNewPlayer, targetScreen->owner);

					client->externalScreenOccupancies.insert(targetScreen);

					// beyond screen
					if (client->params.ps.emitterPos.x < 0)
					{
						client->params.ps.emitterPos.x = client->screenCurrent->size.x - client->params.ps.emitterPos.x;
						client->screenCurrent = targetScreen;
					}
					else if (client->params.ps.emitterPos.x > client->screenCurrent->size.x)
					{
						client->params.ps.emitterPos.x = client->params.ps.emitterPos.x - client->screenCurrent->size.x;
						client->screenCurrent = targetScreen;
					}
				}
			}
		}

		if (!client->externalScreenOccupancies.empty())
		{
			Packet pUpdatePos = p;

			pUpdatePos.mType = UPDATE_POS;
			pUpdatePos.add(client->id);

			for (Screen* s : client->externalScreenOccupancies)
			{
				Server::send(pUpdatePos, s->owner);
			}
		}
	}
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
		if ((*it)->owner == c)
		{
			if (!c->externalScreenOccupancies.empty())
			{
				Packet p;

				p.mType = DELETE_PLAYER;
				p.add(c->id);

				for (Screen* s : c->externalScreenOccupancies)
				{
					Server::send(p, s->owner);
				}
			}

			delete c->screenOwned;

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
