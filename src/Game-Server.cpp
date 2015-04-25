/**
 * Server driver.
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
#include "net\server\Server.h"

#include <iostream>

using namespace std;

Client::ID clientID = 0;
Screen::ID screenID = 0;

std::vector<Screen> screens;
typedef std::vector<Screen>::iterator screen_iterator;

void onConnect(Client* c)
{
	c->id = ++clientID;
	screens.push_back({ ++screenID, c });
}

void onReceive(const Packet& p, Client* c)
{
	switch (p.mType)
	{
	case MessageType::UPDATE_INFO:
		c->params.name = p.mParams.at(0);
		c->params.pp.colorBegin = sf::Color(stoi(p.mParams.at(1)), stoi(p.mParams.at(2)), stoi(p.mParams.at(3)), stoi(p.mParams.at(4)));
		c->params.pp.colorEnd = sf::Color(stoi(p.mParams.at(5)), stoi(p.mParams.at(6)), stoi(p.mParams.at(7)), stoi(p.mParams.at(8)));
		c->params.pp.x = stoi(p.mParams.at(9));
		c->params.pp.y = stoi(p.mParams.at(10));
		break;

	case MessageType::CROSS_SCREENS:
		break;

	case MessageType::UPDATE_POS:
		cout << "UPDATE POS" << endl;
		break;

	case MessageType::REMOVE_TRACKING:
		break;

	default:
		cout << "WUT" << endl;
	}
}

void onDisconnect(Client* c)
{
	for (screen_iterator it = screens.begin(); it != screens.end();)
	{
		if (it->owner == c)
		{
			if (!c->externalScreenOccupancies.empty())
			{
				Packet p;

				p.mType = MessageType::CLIENT_DISCONNECTED;
				p.add(c->id);

				for (Screen* s : c->externalScreenOccupancies)
				{
					Server::send(p, s->owner);
				}
			}

			it = screens.erase(it);
			return;
		}
		else
		{
			++it;
		}
	}
}

int main()
{
	Server server;

	server.setConnectHandler(onConnect);
	server.setReceiveHandler(onReceive);
	server.setDisconnectHandler(onDisconnect);

	server.start(12345);

	cout << "Server running..." << endl;
	cout << "Enter k to kill the server" << endl;

	while (getchar() != 'k');

	cout << "Server stopping..." << endl;

	server.stop();

	while (server.isRunning());

	cout << "Server stopped!" << endl;

	return 0;
}
