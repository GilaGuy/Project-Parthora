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

// TYPEDEFS --------------------------------------------------------------------

typedef sf::Uint16 EntityID;

//-----------------------------------------------------------------------------<

// ENUMS -----------------------------------------------------------------------

enum PacketType
{
	P_INIT,
	P_NEW,
	P_DEL,

	P_NAME,
	P_PARTICLE_PARAMS,
	P_SCREEN,

	P_MOVE,
};

enum Cross
{
	CROSS_NONE,
	CROSS_LEFT,
	CROSS_RIGHT,
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
