#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#include "Protocol.h"
#include "Packet.h"

struct Player;

class PacketCreator
{
public:
	static PacketCreator& Get();

	Packet PlayerInfo
		(
		const sf::Vector2u windowSize,
		const ClientParams& params
		);

	Packet PlayerPos(const sf::Vector2f pos);

	Packet PlayerNew
		(
		const ClientID clientID,
		const Cross crossDir,
		const float offsetX,
		const float ratioY,
		const ClientParams& params
		);

	Packet PlayerDel(const ClientID clientID);

private:
	PacketCreator() {}
	~PacketCreator() {}
};

#endif // PACKETCREATOR_H
