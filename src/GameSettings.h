#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <string>

namespace GameSettings
{
	extern std::string serverIP;
	extern unsigned short serverPort;

	std::string toString();
}

#endif // GAMESETTINGS_H
