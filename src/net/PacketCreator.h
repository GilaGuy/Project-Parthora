#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#include "Packet.h"

struct Screen;

class PacketCreator
{
public:
	static PacketCreator& Create();

	Packet P_Init
		(
			const ClientParams& clientParams,
			const Screen* playerScreen
			);

	Packet P_New
		(
			const EntityID clientID,
			const Cross crossDir,
			const float offsetX,
			const float ratioY,
			const ClientParams& params
			);

	Packet P_Del(const EntityID clientID);

	Packet P_Name(const std::string& name);

	Packet P_ParticleParams(const ParticleParams& particleParams);

	Packet P_Screen(const Screen* screen);

	Packet P_Move(const sf::Vector2i delta);

private:
	PacketCreator() {}
	~PacketCreator() {}
};

#endif // PACKETCREATOR_H
