#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#include "Packet.h"

struct Screen;

class PacketCreator
{
public:
	static PacketCreator& Get();

	Packet PlayerInfo
		(
			const EntityID clientID,
			const ClientParams& params,
			const Screen* playerScreen
			);

	Packet PlayerMove(const sf::Vector2i delta);

	Packet PlayerNew
		(
			const EntityID clientID,
			const Cross crossDir,
			const float offsetX,
			const float ratioY,
			const ClientParams& params
			);

	Packet PlayerDel(const EntityID clientID);

private:
	PacketCreator() {}
	~PacketCreator() {}
};

#endif // PACKETCREATOR_H
