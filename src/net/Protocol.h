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

typedef sf::Uint16 Screen;

enum MessageType
{
	NONE, UPDATE_POS, UPDATE_INFO, CROSS_SCREENS, REMOVE_TRACKING
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
	std::vector<Screen> screensOccupied;
	DynamicClientParams params;
};

#endif // PROTOCOL_H
