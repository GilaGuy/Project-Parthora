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

ClientID clientID = 0;
ScreenID screenID = 0;

vector<Screen*> screens;
typedef vector<Screen*>::iterator screen_iterator_vector;
typedef set<Screen*>::iterator screen_iterator_set;

int getScreenIdx(ClientID ownerID)
{
	for (int s = 0; s < screens.size(); ++s)
	{
		if (screens[s]->owner->params.id == ownerID)
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

	c->params.id = ++clientID;
	c->screenOwned = screens.back();
	c->screenCurrent = c->screenOwned;
}

void onReceive(const Packet& receivedPacket, Client* c)
{

	//if (receivedPacket.mType != PLAYER_MOVE)
	cout << "R> " << receivedPacket.toString() << endl;


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

		// reflect packet to c's ESO
		break;

	case PLAYER_MOVE:
	{
		int clientScreenIdx = getScreenIdx(c->params.id);
		int targetScreenIdx = clientScreenIdx;
		Client* client = screens.at(clientScreenIdx)->owner; // the client that's crossing screens
		Cross cross;

		if (clientScreenIdx == -1)
		{
			cerr << "R> " << "PLAYER_MOVE: Packet sender's screen not found!!" << endl;
		}

		client->params.ps.emitterPos.x += receivedPacket.get<float>(0);
		client->params.ps.emitterPos.y += receivedPacket.get<float>(1);

		cross = checkBeyondBoundaries(client->params.ps.emitterPos, *client->screenCurrent);

		if (cross == NO_CROSS) //>> if within boundaries
		{
			if (!client->externalScreenOccupancies.empty())
			{
				if (client->externalScreenOccupancies.size() > 1)
				{
					for (Screen* s : client->externalScreenOccupancies)
					{
						if (s != client->screenCurrent) Server::send(PacketCreator::Get().PlayerDel(client->params.id), s->owner);
					}
					client->externalScreenOccupancies.clear();
					client->externalScreenOccupancies.insert(client->screenCurrent);
				}
				else
				{
					if (client->externalScreenOccupancies.find(client->screenCurrent)
						== client->externalScreenOccupancies.end())
					{
						cerr << "R> " << "PLAYER_MOVE: The client's one and only external screen occupancy is not their current screen!!" << endl;
					}
				}
			}
		}
		else //>> if beyond boundaries
		{
			float xOffset = 0;

			switch (cross)
			{
			case CROSS_LEFT:
				xOffset = 0 + client->params.ps.emitterPos.x;
				--targetScreenIdx;
				break;

			case CROSS_RIGHT:
				xOffset = client->screenCurrent->size.x - client->params.ps.emitterPos.x;
				++targetScreenIdx;
				break;
			}

			if (targetScreenIdx >= 0 && targetScreenIdx < screens.size())
			{
				Screen* targetScreen = screens.at(targetScreenIdx);

				if (find(client->externalScreenOccupancies.begin(),
					client->externalScreenOccupancies.end(),
					targetScreen)
					== client->externalScreenOccupancies.end()) // if the target screen is not an ESO yet
				{
					Server::send(
						PacketCreator::Get().PlayerNew(
							targetScreen == client->screenOwned ? Client::MYSELF : client->params.id,
							cross,
							xOffset,
							c->params.ps.emitterPos.y / c->screenCurrent->size.y,
							client->params
							)
						, targetScreen->owner);

					client->externalScreenOccupancies.insert(targetScreen);
				}

				cross = checkBeyondScreens(client->params.ps.emitterPos, *client->screenCurrent);

				//>> if beyond screen
				if (cross != NO_CROSS)
				{
					switch (cross)
					{
					case CROSS_LEFT:
						client->params.ps.emitterPos.x = client->screenCurrent->size.x - client->params.ps.emitterPos.x;
						break;

					case CROSS_RIGHT:
						client->params.ps.emitterPos.x = client->params.ps.emitterPos.x - client->screenCurrent->size.x;
						break;
					}

					client->screenCurrent = targetScreen;

					/*
					// the target screen is now our current screen, so we remove it from our list of ESO
					for (screen_iterator_set it = client->externalScreenOccupancies.begin();
					it != client->externalScreenOccupancies.end();)
					{
					if (*it == targetScreen)
					{
					it = client->externalScreenOccupancies.erase(it);
					}
					else
					{
					++it;
					}
					}
					*/
				}
			}
		}

		Packet updatePosPacket = receivedPacket;

		updatePosPacket.mType = PLAYER_MOVE;
		updatePosPacket.add(client->params.id);

		for (Screen* s : client->externalScreenOccupancies)
		{
			Server::send(updatePosPacket, s->owner);
		}

		updatePosPacket.remLast();
		updatePosPacket.add(0);
		Server::send(updatePosPacket, c);
	}
	break;

	default:
		cout << "R> " << "Unknown packet received!" << endl;
	}
}

void onDisconnect(Client* c)
{
	cout << "Client " << c->socket.getRemoteAddress() << " [" << c << "]" << " disconnected!" << endl;

	for (screen_iterator_vector it = screens.begin(); it != screens.end();)
	{
		if ((*it)->owner == c)
		{
			if (!c->externalScreenOccupancies.empty())
			{
				Packet playerDeletePacket = PacketCreator::Get().PlayerDel(c->params.id);

				for (Screen* s : c->externalScreenOccupancies)
				{
					Server::send(playerDeletePacket, s->owner);
				}
			}

			for (const Screen* availableScreen : screens)
			{
				if (!availableScreen->owner->externalScreenOccupancies.empty())
				{
					for (screen_iterator_set it = availableScreen->owner->externalScreenOccupancies.begin();
					it != availableScreen->owner->externalScreenOccupancies.end();)
					{
						if (*it == c->screenOwned /*disconnected client's screen*/)
						{
							it = availableScreen->owner->externalScreenOccupancies.erase(it);
						}
						else
						{
							++it;
						}
					}
				}
			}

			// delete screen owned by the client that disconnected
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
