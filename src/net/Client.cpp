#include "Client.h"

#include "Screen.h"
#include "server\Server.h"

bool Client::remESO(Screen* screenToRemove)
{
	if (!externalScreenOccupancies.empty())
	{
		for (screen_iterator_set it = externalScreenOccupancies.begin();
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

ClientID ClientList::clientID = 0;

ClientList::ClientList() :
	screens(nullptr)
{}

ClientList::ClientList(ScreenList* screenList) :
	screens(screenList)
{}

ClientList::~ClientList()
{}

void ClientList::setScreenList(ScreenList* screenList)
{
	screens = screenList;
}

void ClientList::add(Client* client)
{
	if (!screens || !client) return;

	Screen* newScreen = screens->add();

	newScreen->owner = client;

	client->id = ++clientID;
	client->screenOwned = newScreen;
	client->screenCurrent = client->screenOwned;
}

size_t ClientList::remESOs(Screen* screenToRemove)
{
	size_t count = 0;

	for (Client* client : clients)
	{
		if (client->remESO(screenToRemove)) ++count;
	}

	return count;
}
