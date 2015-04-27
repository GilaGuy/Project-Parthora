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

// ENUMS -----------------------------------------------------------------------

// Listed in chronological order of a simple scenario in which
// a particle from one screen is created, then crosses to another then disconnecting.
enum MessageType
{
	NO_MT, CLIENT_INFO, CROSS_SCREENS, CLIENT_NEW, UPDATE_POS, REMOVE_TRACKING, CLIENT_DISCONNECTED
};

enum CrossingDirection
{
	NO_CD, LEFT, RIGHT
};

//-----------------------------------------------------------------------------<

// PARAMS ----------------------------------------------------------------------

struct ParticleParams
{
	sf::Color colorBegin, colorEnd;
};

struct DynamicClientParams
{
	std::string name;
	float x, y;
	ParticleParams pp;
};

//-----------------------------------------------------------------------------<

// SCREEN ----------------------------------------------------------------------

struct Client;

struct Screen
{
	typedef sf::Uint16 ID;

	ID id;
	Client* owner;
};

//-----------------------------------------------------------------------------<

// CLIENT ----------------------------------------------------------------------

struct Client
{
	sf::TcpSocket socket;

	typedef sf::Uint16 ID;

	static const ID ID_MYSELF = 0;

	ID id;
	size_t screenIdx;
	std::vector<Screen*> externalScreenOccupancies;
	DynamicClientParams params;
};

//-----------------------------------------------------------------------------<

#endif // PROTOCOL_H
