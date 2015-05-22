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
		const DynamicClientParams& params
		);

	Packet PlayerPos(const sf::Vector2f pos);

	Packet PlayerNew
		(
		const Client::ID clientID,
		const Cross crossDir,
		const float offsetX,
		const float ratioY,
		const DynamicClientParams& params
		);

	Packet PlayerDel(const Client::ID clientID);

private:
	PacketCreator() {}
	~PacketCreator() {}
};

#endif // PACKETCREATOR_H
