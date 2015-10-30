/**
 * Shared Client/Server things.
 *
 * @date       April 18, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      This file contains all of the things that both the clients and servers use.
**/

#ifndef NET_SHARED_H
#define NET_SHARED_H

#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

// ENUMS -----------------------------------------------------------------------

// Listed in chronological order of a simple scenario in which
// a particle is created from one screen, crosses to another screen, then disconnects.
enum PacketType
{
	PLAYER_INFO,
	PLAYER_MOVE,
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
	sf::Color colorBegin, colorEnd;
};

struct ClientParams
{
	std::string name;
	sf::Vector2f emitterPos;
	ParticleParams pp;
};

//-----------------------------------------------------------------------------<

#endif // NET_SHARED_H
