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

// TYPEDEFS --------------------------------------------------------------------

typedef sf::Uint16 ClientID;
typedef sf::Uint16 ScreenID;

//-----------------------------------------------------------------------------<

// ENUMS -----------------------------------------------------------------------

// Listed in chronological order of a simple scenario in which
// a particle is created from one screen, crosses to another screen, then disconnects.
enum PacketType
{
	NO_MT,
	PLAYER_INFO,
	PLAYER_POS,
	PLAYER_NEW,
	PLAYER_DEL
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

struct ClientParams
{
	ClientID id;
	std::string name;
	ParticleParams ps;
};

//-----------------------------------------------------------------------------<

// SCREEN ----------------------------------------------------------------------

struct Client;

struct Screen
{
	Client* owner;

	ScreenID id;
	sf::Vector2u size;
	float boundaryLeft, boundaryRight;
};

//-----------------------------------------------------------------------------<

// CLIENT ----------------------------------------------------------------------

struct Client
{
	static const ClientID MYSELF = 0;

	sf::TcpSocket socket;

	Screen *screenOwned, *screenCurrent;
	std::set<Screen*> externalScreenOccupancies;
	ClientParams params;
};

//-----------------------------------------------------------------------------<

#endif // PROTOCOL_H
