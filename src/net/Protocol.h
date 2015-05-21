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
#include <set>

// ENUMS -----------------------------------------------------------------------

// Listed in chronological order of a simple scenario in which
// a particle is created from one screen, crosses to another screen, then disconnects.
enum PacketType
{
	NO_MT,
	PLAYER_INFO,
	UPDATE_POS,
	NEW_PLAYER,
	DELETE_PLAYER
};

enum Cross
{
	NO_CROSS,
	CROSS_LEFT,
	CROSS_RIGHT
};

//-----------------------------------------------------------------------------<

// PARAMS ----------------------------------------------------------------------

struct ParticleParams
{
	sf::Vector2f emitterPos;
	sf::Color colorBegin, colorEnd;
};

struct DynamicClientParams
{
	std::string name;
	ParticleParams ps;
};

//-----------------------------------------------------------------------------<

// SCREEN ----------------------------------------------------------------------

struct Client;

struct Screen
{
	typedef sf::Uint16 ID;

	ID id;
	Client* owner;
	sf::Vector2u size;
	float boundaryLeft, boundaryRight;
};

//-----------------------------------------------------------------------------<

// CLIENT ----------------------------------------------------------------------

struct Client
{
	sf::TcpSocket socket;

	typedef sf::Uint16 ID;

	static const ID ID_MYSELF = 0;

	ID id;
	Screen *screenOwned, *screenCurrent;
	std::set<Screen*> externalScreenOccupancies;
	DynamicClientParams params;
};

//-----------------------------------------------------------------------------<

#endif // PROTOCOL_H
