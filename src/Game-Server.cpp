#include <iostream>
#include "net\server\Server.h"

using namespace std;

void onReceive(const Packet& p, Client* c)
{
	switch (p.mType)
	{
	case MessageType::UPDATE_INFO:
		c->params.name = p.mParams.at(0);
		c->params.pp.colorBegin = sf::Color(std::stoi(p.mParams.at(1)), std::stoi(p.mParams.at(2)), std::stoi(p.mParams.at(3)), std::stoi(p.mParams.at(4)));
		c->params.pp.colorEnd = sf::Color(std::stoi(p.mParams.at(5)), std::stoi(p.mParams.at(6)), std::stoi(p.mParams.at(7)), std::stoi(p.mParams.at(8)));
		c->params.pp.x = std::stoi(p.mParams.at(9));
		c->params.pp.y = std::stoi(p.mParams.at(10));
		break;

	case MessageType::UPDATE_POS:
		cout << "UPDATE POS" << endl;
		break;

	default:
		cout << "WUT" << endl;
	}
}

int main()
{
	Server server(onReceive);

	server.start(12345);

	cout << "Server running..." << endl;
	cout << "Enter k to kill the server" << endl;

	while (getchar() != 'k');

	server.stop();

	cout << "Server stopping..." << endl;

	while (server.isRunning());

	return 0;
}
