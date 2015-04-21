/**
 * This file contains all of the common things that will be used by both the server and the client.
 *
 * @date       April 18, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <SFML/Network.hpp>

typedef unsigned int Screen;

enum MessageType
{
	NONE, UPDATE_POS, UPDATE_INFO, CROSS_SCREENS
};

struct Color
{
	char r, g, b;
};

struct ParticleParams
{
	float x, y;
	Color colorBegin, colorEnd;
};

struct DynamicClientParams
{
	std::string name;
	ParticleParams pp;
};

struct Client
{
	sf::TcpSocket socket;
	Screen ownerScreen, currentScreen;
	DynamicClientParams params;
};

#endif // PROTOCOL_H
