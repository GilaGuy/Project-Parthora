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

// STRUCTS ---------------------------------------------------------------------

struct Client;

struct Screen
{
	Client* owner;

	ScreenID id;
	sf::Vector2u size;
	float boundaryLeft, boundaryRight;
};

struct Client
{
	static const ClientID MYSELF = 0;

	sf::TcpSocket socket;

	Screen *screenOwned, *screenCurrent;
	std::set<Screen*> externalScreenOccupancies;
	ClientParams params;
};

//-----------------------------------------------------------------------------<

// HELPER FUNCTIONS ------------------------------------------------------------

Cross checkBeyondBoundaries(sf::Vector2f coords, const Screen& s);
Cross checkBeyondScreens(sf::Vector2f coords, const Screen& s);

//-----------------------------------------------------------------------------<

#endif // PROTOCOL_H
