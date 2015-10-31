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
#include "net/Shared.h"
#include "net/PacketCreator.h"
#include "net/entities/Client.h"
#include "net/entities/Screen.h"
#include "GameSettings.h"

#include <iostream>

using namespace std;

void onConnect(Client* c)
{
	cout << "Client " << c->id << " [" << c->socket.getRemoteAddress() << "] " << "connected!" << endl;
}

void onReceive(const Packet& receivedPacket, Client* client)
{
	switch (receivedPacket.type)
	{
	case PLAYER_INFO:
	{
		// sync params
		client->params.name = receivedPacket.get(1);
		client->params.pp.colorBegin = sf::Color(receivedPacket.get<sf::Uint32>(2));
		client->params.pp.colorEnd = sf::Color(receivedPacket.get<sf::Uint32>(3));

		// sync screen sizes/boundaries
		client->screenOwned->size.x = receivedPacket.get<unsigned int>(4);
		client->screenOwned->size.y = receivedPacket.get<unsigned int>(5);
		client->screenOwned->boundaryLeft = client->screenOwned->size.x * 0.125f;
		client->screenOwned->boundaryRight = client->screenOwned->size.x - client->screenOwned->boundaryLeft;

		// center the emitter's position
		client->params.emitterPos.x = client->screenOwned->size.x * 0.5f;
		client->params.emitterPos.y = client->screenOwned->size.y * 0.5f;

		Packet playerInfoPacket = receivedPacket;

		playerInfoPacket.remLast();
		playerInfoPacket.remLast();

		Server::send(playerInfoPacket, client);

		// reflect packet to client's ESO
		if (!client->externalScreenOccupancies.empty())
		{
			playerInfoPacket.replace(0, Packet::ToString(client->id));

			for (Screen* s : client->externalScreenOccupancies)
			{
				Server::send(playerInfoPacket, s->owner);
			}
		}
	}
	break;

	case PLAYER_MOVE:
	{
		Screen* targetScreen;
		Cross cross;

		client->params.emitterPos.x += receivedPacket.get<float>(0);
		client->params.emitterPos.y += receivedPacket.get<float>(1);

		cross = client->screenCurrent->checkBeyondBoundaries(client->params.emitterPos);

		if (cross == NO_CROSS) //>> if within boundaries
		{
			if (client->screenCurrent == client->screenOwned)
			{
				for (Screen* s : client->externalScreenOccupancies)
				{
					Server::send(PacketCreator::Get().PlayerDel(client->id), s->owner);
				}
				client->externalScreenOccupancies.clear();
			}
			else
			{
				for (Client::ESOListIter it = client->externalScreenOccupancies.begin();
				it != client->externalScreenOccupancies.end();)
				{
					if (*it != client->screenCurrent)
					{
						it = client->externalScreenOccupancies.erase(it);
					}
					else
					{
						++it;
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
				targetScreen = client->screenCurrent->prev;
				break;

			case CROSS_RIGHT:
				xOffset = client->screenCurrent->size.x - client->params.emitterPos.x;
				targetScreen = client->screenCurrent->next;
				break;
			}

			if (targetScreen)
			{
				if (client->externalScreenOccupancies.find(targetScreen) == client->externalScreenOccupancies.end()) // if the target screen is not an ESO yet
				{
					Server::send(
						PacketCreator::Get().PlayerNew(
							targetScreen == client->screenOwned ? Client::MYSELF : client->id,
							cross,
							xOffset,
							client->params.emitterPos.y / client->screenCurrent->size.y,
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
				}
			}
		}

		Packet updatePosPacket = receivedPacket;

		updatePosPacket.add(Client::MYSELF);
		Server::send(updatePosPacket, client);

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
}

int main(int argc, char const *argv[])
{
	/* // Screen & Client Manager test
	ScreenManager sm;
	ClientManager cm(&sm);
	Screen* curr = nullptr;
	bool success;

	cm.add(new Client());
	cm.add(new Client());
	cm.add(new Client());

	sm.print();

	success = sm.rem(1);

	sm.print();

	sm.clear();

	sm.print();

	return getchar();
	*/

	if (argc > 1)
	{
		GameSettings::serverPort = static_cast<unsigned short>(stoul(argv[1]));
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
	cout << "Enter k to kill the server!" << endl;
	cout << std::string(80, '-');

	while (getchar() != 'k');

	cout << "Server stopping..." << endl;

	server.stop();

	while (server.isRunning());

	cout << "Server stopped!" << endl;

	return EXIT_SUCCESS;
}
