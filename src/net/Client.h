#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Network.hpp>
#include <vector>
#include <set>
#include "Protocol.h"

struct Screen;
class ScreenList;

struct Client
{
	typedef std::set<Screen*>::iterator screen_iterator_set;

	static const ClientID MYSELF = 0;

	bool remESO(Screen* screenToRemove);

	sf::TcpSocket socket;

	Screen *screenOwned, *screenCurrent;
	std::set<Screen*> externalScreenOccupancies;

	ClientID id;
	ClientParams params;
};

class ClientList
{
public:
	ClientList();
	ClientList(ScreenList* screenList);
	~ClientList();

	void setScreenList(ScreenList* screenList);
	void add(Client* client);
	size_t remESOs(Screen* screenToRemove);

private:
	static ClientID clientID;

	ScreenList* screens;
	std::vector<Client*> clients;
};

#endif /* CLIENT_H */
