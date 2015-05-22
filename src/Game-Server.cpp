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
#include "net/server/Server.h"
#include "net/Protocol.h"
#include "GameSettings.h"
#include "net/PacketCreator.h"

#include <iostream>

using namespace std;

Client::ID clientID = 0;
Screen::ID screenID = 0;

vector<Screen*> screens;
typedef vector<Screen*>::iterator screen_iterator;

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

void onReceive(const Packet& receivedPacket, Client* c)
{

	if (receivedPacket.mType != PLAYER_POS)
		cout << "onReceive> " << receivedPacket.toString() << endl;


	switch (receivedPacket.mType)
	{
	case PLAYER_INFO:
		c->params.name = receivedPacket.get(0);
		c->screenOwned->size.x = receivedPacket.get<unsigned int>(1);
		c->screenOwned->size.y = receivedPacket.get<unsigned int>(2);
		c->params.ps.emitterPos.x = receivedPacket.get<float>(3);
		c->params.ps.emitterPos.y = receivedPacket.get<float>(4);
		c->screenOwned->boundaryLeft = c->screenOwned->size.x * 0.125f;
		c->screenOwned->boundaryRight = c->screenOwned->size.x - c->screenOwned->boundaryLeft;
		c->params.ps.colorBegin = sf::Color(receivedPacket.get<sf::Uint32>(5), receivedPacket.get<sf::Uint32>(6), receivedPacket.get<sf::Uint32>(7), receivedPacket.get<sf::Uint32>(8));
		c->params.ps.colorEnd = sf::Color(receivedPacket.get<sf::Uint32>(9), receivedPacket.get<sf::Uint32>(10), receivedPacket.get<sf::Uint32>(11), receivedPacket.get<sf::Uint32>(12));
		break;

	case PLAYER_POS:
	{
		int clientScreenIdx = getScreenIdx(c->id);
		int targetScreenIdx = clientScreenIdx;
		Client* client = screens.at(clientScreenIdx)->owner; // the client that's crossing screens

		if (clientScreenIdx == -1)
		{
			cerr << "onReceive> " << "PLAYER_POS: Packet sender's screen not found!!" << endl;
		}

		client->params.ps.emitterPos.x += receivedPacket.get<float>(0);
		client->params.ps.emitterPos.y += receivedPacket.get<float>(1);

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
				if (client->externalScreenOccupancies.size() > 1)
				{
					for (Screen* s : client->externalScreenOccupancies)
					{
						if (s != client->screenCurrent) Server::send(PacketCreator::Get().PlayerDel(client->id), s->owner);
					}
					client->externalScreenOccupancies.clear();
					client->externalScreenOccupancies.insert(client->screenCurrent);
				}
				else
				{
					if (client->externalScreenOccupancies.find(client->screenCurrent)
						== client->externalScreenOccupancies.end())
					{
						cerr << "onReceive> " << "PLAYER_POS: The client's one and only external screen occupancy is not their current screen!!" << endl;
					}
				}
			}
		}
		else // beyond boundaries
		{
			if (targetScreenIdx >= 0 && targetScreenIdx < screens.size())
			{
				Screen* targetScreen = screens.at(targetScreenIdx);

				if (find(client->externalScreenOccupancies.begin(),
					client->externalScreenOccupancies.end(),
					targetScreen)
					== client->externalScreenOccupancies.end())
				{
					Server::send(
						PacketCreator::Get().PlayerNew(
						targetScreen == client->screenOwned ? Client::ID_MYSELF : client->id,
						cross,
						xOffset,
						c->params.ps.emitterPos.y / c->screenCurrent->size.y,
						client->params
						)
						, targetScreen->owner);

					client->externalScreenOccupancies.insert(targetScreen);
				}

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

		if (!client->externalScreenOccupancies.empty())
		{
			Packet updatePosPacket = receivedPacket;

			updatePosPacket.mType = PLAYER_POS;
			updatePosPacket.add(client->id);

			for (Screen* s : client->externalScreenOccupancies)
			{
				Server::send(updatePosPacket, s->owner);
			}
		}
	}
	break;

	default:
		cout << "onReceive> " << "Unknown packet received!" << endl;
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
				Packet playerDeletePacket = PacketCreator::Get().PlayerDel(c->id);

				for (Screen* s : c->externalScreenOccupancies)
				{
					Server::send(playerDeletePacket, s->owner);
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

int main(int argc, char const *argv[])
{
	if (argc > 1)
	{
		GameSettings::serverPort = stoul(argv[1]);
	}

	Server server;

	server.setConnectHandler(onConnect);
	server.setReceiveHandler(onReceive);
	server.setDisconnectHandler(onDisconnect);

	if (!server.start(GameSettings::serverPort))
	{
		cerr << "Server failed to start!" << endl;
		return EXIT_FAILURE;
	}

	cout << "[-Project Parthora-]" << endl;
	cout << "by Melvin Loho" << endl;
	cout << endl;

	cout << "Server running on..." << GameSettings::toString() << endl;
	cout << "Enter k to kill the server" << endl;
	cout << endl;

	while (getchar() != 'k');

	cout << "Server stopping..." << endl;

	server.stop();

	while (server.isRunning());

	cout << "Server stopped!" << endl;

	return EXIT_SUCCESS;
}
