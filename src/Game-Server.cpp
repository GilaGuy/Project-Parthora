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

Server server;

// Sync self
void reflectPacketToSender(const Packet& packet, Client* sender)
{
	Server::Send(packet, sender);
}

// Sync ESO
void reflectPacketToThoseAffectedBySender(const Packet& packet, Client* sender)
{
	if (sender->hasESOs())
	{
		for (Screen* s : sender->externalScreenOccupancies)
		{
			Server::Send(packet, s->owner);
		}
	}
}

// Sync Inverse ESO
void reflectPacketToThoseInSenderScreen(const Packet& packet, const Client* sender)
{
	for (Client* c : server.getClients())
	{
		if (c->screenCurrent == sender->screenOwned)
		{
			Server::Send(packet, c);
		}
	}
}

void onConnect(Client* client)
{
	cout << "Client " << client->id << " [" << client->socket.getRemoteAddress() << "] " << "connected!" << endl;
}

void onReceive(const Packet& receivedPacket, Client* sender)
{
	switch (receivedPacket.type)
	{
	case P_INIT:
	{
		// sync params
		sender->params.name = receivedPacket.get(0);
		sender->params.pp.colorBegin = sf::Color(receivedPacket.get<sf::Uint32>(1));
		sender->params.pp.colorEnd = sf::Color(receivedPacket.get<sf::Uint32>(2));

		// sync screen sizes/boundaries
		sender->screenOwned->size.x = receivedPacket.get<unsigned int>(3);
		sender->screenOwned->size.y = receivedPacket.get<unsigned int>(4);
		sender->screenOwned->boundaryLeft = sender->screenOwned->size.x * 0.25f;
		sender->screenOwned->boundaryRight = sender->screenOwned->size.x - sender->screenOwned->boundaryLeft;

		// center the emitter's position
		sender->params.emitterPos.x = sender->screenOwned->size.x * 0.5f;
		sender->params.emitterPos.y = sender->screenOwned->size.y * 0.5f;

		// send back the packet (might want to remove the contents unless they are changed)
		reflectPacketToSender(receivedPacket, sender);
	}
	break;

	case P_NAME:
	{
		sender->params.name = receivedPacket.get(0);

		Packet reflectPacket = receivedPacket;

		reflectPacket.add(Client::MYSELF);
		reflectPacketToSender(reflectPacket, sender);

		reflectPacket.replace(reflectPacket.last(), sender->id);
		reflectPacketToThoseAffectedBySender(reflectPacket, sender);
	}
	break;

	case P_PARTICLE_PARAMS:
	{
		sender->params.pp.colorBegin = sf::Color(receivedPacket.get<sf::Uint32>(0));
		sender->params.pp.colorEnd = sf::Color(receivedPacket.get<sf::Uint32>(1));

		Packet reflectPacket = receivedPacket;

		reflectPacket.add(Client::MYSELF);
		reflectPacketToSender(reflectPacket, sender);

		reflectPacket.replace(reflectPacket.last(), sender->id);
		reflectPacketToThoseAffectedBySender(reflectPacket, sender);
	}
	break;

	case P_SCREEN:
	{
		sender->screenOwned->size.x = receivedPacket.get<unsigned int>(0);
		sender->screenOwned->size.y = receivedPacket.get<unsigned int>(1);
		sender->screenOwned->boundaryLeft = sender->screenOwned->size.x * 0.125f;
		sender->screenOwned->boundaryRight = sender->screenOwned->size.x - sender->screenOwned->boundaryLeft;

		Packet reflectPacket = receivedPacket;

		reflectPacketToSender(reflectPacket, sender);
		reflectPacketToThoseAffectedBySender(reflectPacket, sender);
	}
	break;

	case P_MOVE:
	{
		Cross cross;

		sender->params.emitterPos.x += receivedPacket.get<float>(0);
		sender->params.emitterPos.y += receivedPacket.get<float>(1);

		cross = sender->screenCurrent->checkBeyondBoundaries(sender->params.emitterPos);

		if (cross == CROSS_NONE) //>> if within boundaries
		{
			if (sender->hasESOs())
			{
				if (sender->screenCurrent == sender->screenOwned) // coming home
				{
					for (Screen* s : sender->externalScreenOccupancies)
					{
						Server::Send(PacketCreator::Create().P_Del(sender->id), s->owner);
					}
					sender->externalScreenOccupancies.clear();
				}
				else
				{
					for (Client::ESOListIter it = sender->externalScreenOccupancies.begin();
					it != sender->externalScreenOccupancies.end();)
					{
						if (*it != sender->screenCurrent)
						{
							Server::Send(PacketCreator::Create().P_Del(sender->id), (*it)->owner);
							it = sender->externalScreenOccupancies.erase(it);
						}
						else
						{
							++it;
						}
					}
				}
			}
		}
		else //>> if beyond boundaries
		{
			float xOffset;
			Screen* targetScreen;

			switch (cross)
			{
			case CROSS_LEFT:
				xOffset = 0 + sender->params.emitterPos.x;
				targetScreen = sender->screenCurrent->prev;
				break;

			case CROSS_RIGHT:
				xOffset = sender->screenCurrent->size.x - sender->params.emitterPos.x;
				targetScreen = sender->screenCurrent->next;
				break;
			}

			if (targetScreen)
			{
				if (sender->externalScreenOccupancies.find(targetScreen) == sender->externalScreenOccupancies.end()) // if the target screen is not an ESO yet
				{
					Server::Send(
						PacketCreator::Create().P_New(
							targetScreen == sender->screenOwned ? Client::MYSELF : sender->id,
							cross,
							xOffset,
							sender->params.emitterPos.y / sender->screenCurrent->size.y,
							sender->params
							)
						, targetScreen->owner);

					sender->externalScreenOccupancies.insert(targetScreen);
				}

				cross = sender->screenCurrent->checkBeyondScreens(sender->params.emitterPos);

				//>> if beyond screen
				if (cross != CROSS_NONE)
				{
					switch (cross)
					{
					case CROSS_LEFT:
						sender->params.emitterPos.x = sender->screenCurrent->size.x - sender->params.emitterPos.x;
						break;

					case CROSS_RIGHT:
						sender->params.emitterPos.x = sender->params.emitterPos.x - sender->screenCurrent->size.x;
						break;
					}

					sender->screenCurrent = targetScreen;

					// update sender's screen properties
					Server::Send(PacketCreator::Create().P_Screen(sender->screenCurrent), sender);
				}
			}
		}

		Packet reflectPacket = receivedPacket;

		reflectPacket.add(Client::MYSELF);
		reflectPacketToSender(reflectPacket, sender);

		reflectPacket.replace(reflectPacket.last(), sender->id);
		reflectPacketToThoseAffectedBySender(reflectPacket, sender);
	}
	break;
	}
}

void onDisconnect(Client* client)
{
	cout << "Client " << client->id << " [" << client->socket.getRemoteAddress() << "] " << "disconnected!" << endl;

	// tell client's ESOs to delete the player
	if (client->hasESOs())
	{
		Packet playerDeletePacket = PacketCreator::Create().P_Del(client->id);

		for (Screen* s : client->externalScreenOccupancies)
		{
			Server::Send(playerDeletePacket, s->owner);
		}
	}

	if (server.getClients().size() > 1)
	{
		for (Client* c : server.getClients())
		{
			if (c->screenCurrent == client->screenOwned)
			{
				//@TODO move player to a neighbouring screen
			}
		}
	}
}

int main(int argc, char const *argv[])
{
	if (argc > 1)
	{
		GameSettings::serverPort = static_cast<unsigned short>(stoul(argv[1]));
	}

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
