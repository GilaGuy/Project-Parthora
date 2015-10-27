#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#include "Protocol.h"
#include "Packet.h"
#include "Screen.h"

class PacketCreator
{
public:
	static PacketCreator& Get();

	Packet PlayerInfo
		(
			const ClientID clientID,
			const ClientParams& params,
			const Screen& playerScreen
			);

	Packet PlayerMove(const sf::Vector2i delta);

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
