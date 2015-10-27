/**
 * Client and its manager.
 *
 * @date       October 26, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      Represents a client connected to the server.
 *             Includes its main network socket, screen variables and client params.
 *             Includes the ClientManager to manage a set of clients.
 */

 // @TODO: Make ClientManager be used in Server instead of their own vector of Clients.
 //        Then, there'll only be 1 container of Clients instead of 2 (in Server & ClientManager).
 //        Note to self: Don't forget to move the memory management from Server to ClientManager.

#include "Client.h"

#include "Screen.h"
#include "../server/Server.h"

bool Client::remESO(Screen* screenToRemove)
{
	if (!externalScreenOccupancies.empty())
	{
		for (screen_iterator it = externalScreenOccupancies.begin();
		it != externalScreenOccupancies.end();)
		{
			if (*it == screenToRemove)
			{
				it = externalScreenOccupancies.erase(it);
				return true;
			}
			else
			{
				++it;
			}
		}
	}

	return false;
}

ClientID ClientManager::clientID = 0;

ClientManager::ClientManager() :
	screens(nullptr)
{}

ClientManager::ClientManager(ScreenManager* screenList) :
	screens(screenList)
{}

ClientManager::~ClientManager()
{}

void ClientManager::setScreenList(ScreenManager* screenList)
{
	screens = screenList;
}

void ClientManager::add(Client* client)
{
	if (!screens || !client) return;

	Screen* newScreen = screens->add();

	newScreen->owner = client;

	client->id = ++clientID;
	client->screenOwned = newScreen;
	client->screenCurrent = client->screenOwned;
}

void ClientManager::rem(Client* client)
{
	for (ClientListIter it = clients.begin(); it != clients.end();)
	{
		if (*it == client)
		{
			clients.erase(it);
			return;
		}
	}
}

size_t ClientManager::remESOs(Screen* screenToRemove)
{
	size_t count = 0;

	for (Client* client : clients)
	{
		if (client->remESO(screenToRemove)) ++count;
	}

	return count;
}

void ClientManager::clear()
{
	clients.clear();
}
