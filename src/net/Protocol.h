#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <SFML/Graphics.hpp>

// TYPEDEFS --------------------------------------------------------------------

typedef sf::Uint16 ClientID;

//-----------------------------------------------------------------------------<

// ENUMS -----------------------------------------------------------------------

// Listed in chronological order of a simple scenario in which
// a particle is created from one screen, crosses to another screen, then disconnects.
enum PacketType
{
	NO_MT,
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

#endif // PROTOCOL_H
