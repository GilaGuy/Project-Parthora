#include "GameSettings.h"

std::string GameSettings::serverIP = "localhost";
unsigned short GameSettings::serverPort = 12345;

std::string GameSettings::toString()
{
	return "\n"
		"IP:   " + serverIP + "\n" +
		"Port: " + std::to_string(serverPort) + "\n"
		;
}
