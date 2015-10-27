#ifndef CLIENT_H
#define CLIENT_H

#include <set>
#include <SFML/Network.hpp>
#include "../Protocol.h"

struct Screen;
class ScreenManager;

struct Client
{
	typedef std::set<Screen*>::iterator screen_iterator;

	static const ClientID MYSELF = 0;

	bool remESO(Screen* screenToRemove);

	sf::TcpSocket socket;

	Screen *screenOwned, *screenCurrent;
	std::set<Screen*> externalScreenOccupancies;

	ClientID id;
	ClientParams params;
};

class ClientManager
{
	typedef std::set<Client*> ClientList;
	typedef ClientList::iterator ClientListIter;

public:
	ClientManager();
	ClientManager(ScreenManager* screenList);
	~ClientManager();

	void setScreenList(ScreenManager* screenList);
	void add(Client* client);
	void rem(Client* client);
	size_t remESOs(Screen* screenToRemove);
	void clear();

private:
	static ClientID clientID;

	ScreenManager* screens;
	ClientList clients;
};

#endif /* CLIENT_H */
