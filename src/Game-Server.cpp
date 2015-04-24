#include <iostream>
#include "net\server\Server.h"

using namespace std;

void onReceive(const Packet& p, Client* c)
{
	switch (p.mType)
	{
	case MessageType::UPDATE_POS:
		cout << "UPDATE POS haha" << endl;
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
