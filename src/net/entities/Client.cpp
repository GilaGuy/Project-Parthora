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
	ESOListIter it = externalScreenOccupancies.find(screenToRemove);

	if (it == externalScreenOccupancies.end()) return false;

	externalScreenOccupancies.erase(it);
	return true;
}

EntityID ClientManager::ID_ENTITY = 1;

ClientManager::ClientManager()
{}

ClientManager::~ClientManager()
{
	screens.clear();
	clear();
}

Client* ClientManager::add()
{
	Client* newClient = *clients.insert(new Client()).first;
	Screen* newScreen = screens.add();

	newScreen->owner = newClient;

	newClient->id = ID_ENTITY++;
	newClient->screenOwned = newScreen;
	newClient->screenCurrent = newClient->screenOwned;

	return newClient;
}

bool ClientManager::rem(EntityID id)
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
	ListIter it = clients.find(c);

	if (it == clients.end()) return false;

	rem(it);
	return true;
}

ClientManager::ListIter ClientManager::rem(ListIter it)
{
	Client* toRemove = *it;

	// remove the screen that the disconnected client owns from other clients' ESOs
	remESOs(toRemove->screenOwned);

	// delete the screen owned by the client that disconnected
	screens.rem(toRemove->id);

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
