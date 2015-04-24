/**
 * Protocol specifications.
 *
 * @date       April 18, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      This file contains all of the common things that will be used by the server and client.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

typedef unsigned int Screen;

enum MessageType
{
	NONE, UPDATE_POS, UPDATE_INFO, CROSS_SCREENS
};

struct ParticleParams
{
	sf::Color colorBegin, colorEnd;
	float x, y;
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
