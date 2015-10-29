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
				externalScreenOccupancies.erase(it);
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

ClientID ClientManager::ID_CLIENT = 0;

ClientManager::ClientManager() :
	screens(nullptr)
{}

ClientManager::ClientManager(ScreenManager* screenList) :
	screens(screenList)
{}

ClientManager::~ClientManager()
{
	if (screens)
	{
		screens->clear();
	}

	clear();
}

void ClientManager::setScreenList(ScreenManager* screenList)
{
	if (!screenList) return;

	if (screens)
	{
		screens->clear();
	}

	clear();

	screens = screenList;
}

Client* ClientManager::add()
{
	if (!screens) return nullptr;

	Client* newClient = *clients.insert(new Client()).first;
	Screen* newScreen = screens->add();

	newScreen->owner = newClient;

	newClient->id = ID_CLIENT++;
	newClient->screenOwned = newScreen;
	newClient->screenCurrent = newClient->screenOwned;

	return newClient;
}

bool ClientManager::rem(ClientID id)
{
	for (ListIter it = clients.begin(); it != clients.end();)
	{
		if ((*it)->id == id)
		{
			rem(it);
			return true;
		}
		else
		{
			++it;
		}
	}

	return false;
}

bool ClientManager::rem(Client* c)
{
	for (ListIter it = clients.begin(); it != clients.end();)
	{
		if (*it == c)
		{
			rem(it);
			return true;
		}
		else
		{
			++it;
		}
	}

	return false;
}

ClientManager::ListIter ClientManager::rem(ListIter it)
{
	Client* toRemove = *it;

	// remove the screen that the disconnected client owns from other clients' ESOs
	remESOs(toRemove->screenOwned);

	// delete the screen owned by the client that disconnected
	screens->rem(toRemove->id);

	// disconnect its socket
	toRemove->socket.disconnect();

	// finally delete the client object
	delete toRemove; toRemove = nullptr;

	return clients.erase(it);
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
	for (ListIter it = clients.begin(); it != clients.end();) it = rem(it);

	clients.clear();
}
