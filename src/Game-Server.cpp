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
#include "net/PacketCreator.h"
#include "net/Screen.h"
#include "net/Client.h"
#include "GameSettings.h"

#include <iostream>

using namespace std;

ScreenList screens;
ClientList clients;

void onConnect(Client* c)
{
	cout << "Client " << c->id << " [" << c->socket.getRemoteAddress() << "] " << "connected!" << endl;

	clients.add(c);
}

void onReceive(const Packet& receivedPacket, Client* c)
{
	switch (receivedPacket.mType)
	{
	case PLAYER_INFO:
	{
		c->params.name = receivedPacket.get(1);
		c->params.pp.colorBegin = sf::Color(receivedPacket.get<sf::Uint32>(2));
		c->params.pp.colorEnd = sf::Color(receivedPacket.get<sf::Uint32>(3));

		c->screenOwned->size.x = receivedPacket.get<unsigned int>(4);
		c->screenOwned->size.y = receivedPacket.get<unsigned int>(5);
		c->screenOwned->boundaryLeft = c->screenOwned->size.x * 0.125f;
		c->screenOwned->boundaryRight = c->screenOwned->size.x - c->screenOwned->boundaryLeft;

		Packet playerInfoPacket = receivedPacket;

		playerInfoPacket.remLast();
		playerInfoPacket.remLast();

		Server::send(playerInfoPacket, c);

		// reflect packet to c's ESO
		if (!c->externalScreenOccupancies.empty())
		{
			playerInfoPacket.replace(0, Packet::ToString(c->id));

			for (Screen* s : c->externalScreenOccupancies)
			{
				Server::send(playerInfoPacket, s->owner);
			}
		}
	}
	break;

	case PLAYER_MOVE:
	{
		Screen* targetScreen;
		Client* client = c;
		Cross cross;

		client->params.emitterPos.x += receivedPacket.get<float>(0);
		client->params.emitterPos.y += receivedPacket.get<float>(1);

		cross = client->screenCurrent->checkBeyondBoundaries(client->params.emitterPos);

		if (cross == NO_CROSS) //>> if within boundaries
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
						cerr << "PLAYER_MOVE: The client's one and only external screen occupancy is not their current screen!!" << endl;
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
				xOffset = 0 + client->params.emitterPos.x;
				targetScreen = c->screenCurrent->prev;
				break;

			case CROSS_RIGHT:
				xOffset = client->screenCurrent->size.x - client->params.emitterPos.x;
				targetScreen = c->screenCurrent->next;
				break;
			}

			if (targetScreen)
			{
				if (find(client->externalScreenOccupancies.begin(),
					client->externalScreenOccupancies.end(),
					targetScreen)
					== client->externalScreenOccupancies.end()) // if the target screen is not an ESO yet
				{
					Server::send(
						PacketCreator::Get().PlayerNew(
							targetScreen == client->screenOwned ? Client::MYSELF : client->id,
							cross,
							xOffset,
							c->params.emitterPos.y / c->screenCurrent->size.y,
							client->params
							)
						, targetScreen->owner);

					client->externalScreenOccupancies.insert(targetScreen);
				}

				cross = client->screenCurrent->checkBeyondScreens(client->params.emitterPos);

				//>> if beyond screen
				if (cross != NO_CROSS)
				{
					switch (cross)
					{
					case CROSS_LEFT:
						client->params.emitterPos.x = client->screenCurrent->size.x - client->params.emitterPos.x;
						break;

					case CROSS_RIGHT:
						client->params.emitterPos.x = client->params.emitterPos.x - client->screenCurrent->size.x;
						break;
					}

					client->screenCurrent = targetScreen;

					///*
					// the target screen is now our current screen, so we remove it from our list of ESO
					for (Client::screen_iterator_set it = client->externalScreenOccupancies.begin();
					it != client->externalScreenOccupancies.end();)
					{
						if (*it == targetScreen)
						{
							it = client->externalScreenOccupancies.erase(it);
							return;
						}
						else
						{
							++it;
						}
					}
					//*/
				}
			}
		}

		Packet updatePosPacket = receivedPacket;

		updatePosPacket.add(Client::MYSELF);
		Server::send(updatePosPacket, c);

		if (!client->externalScreenOccupancies.empty())
		{
			updatePosPacket.remLast();
			updatePosPacket.add(client->id);
			for (Screen* s : client->externalScreenOccupancies)
			{
				Server::send(updatePosPacket, s->owner);
			}
		}
	}
	break;
	}
}

void onDisconnect(Client* c)
{
	cout << "Client " << c->id << " [" << c->socket.getRemoteAddress() << "] " << "disconnected!" << endl;

	// tell client's ESOs to delete the player
	if (!c->externalScreenOccupancies.empty())
	{
		Packet playerDeletePacket = PacketCreator::Get().PlayerDel(c->id);

		for (Screen* s : c->externalScreenOccupancies)
		{
			Server::send(playerDeletePacket, s->owner);
		}
	}

	// remove the screen that the disconnected client owns from other clients' ESOs
	cout << "Client " << c->id << " > " << "Removed " << clients.remESOs(c->screenOwned) << " ESOs." << endl;

	// delete the screen owned by the client that disconnected
	screens.rem(c->id);
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

	clients.setScreenList(&screens);

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
