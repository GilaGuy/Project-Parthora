/**
* Creates all of the packets needed for communication between the clients and the server.
*
* @date       May 21, 2015
*
* @revisions
*
* @designer   Melvin Loho
*
* @programmer Melvin Loho
*
* @notes
*/

#include "PacketCreator.h"

#include "entities/Screen.h"

PacketCreator& PacketCreator::Create()
{
	static PacketCreator pc;
	return pc;
}

Packet PacketCreator::P_Init(
	const ClientParams& clientParams,
	const Screen* playerScreen)
{
	Packet p;
	p.type = P_INIT;

	p.add(clientParams.name); //0
	p.add(clientParams.pp.colorBegin.toInteger()); //1
	p.add(clientParams.pp.colorEnd.toInteger()); //2

	p.add(playerScreen->size.x); //3
	p.add(playerScreen->size.y); //4

	return p;
}

Packet PacketCreator::P_New(
	const EntityID clientID,
	const Cross crossDir,
	const float offsetX,
	const float ratioY,
	const ClientParams& params)
{
	Packet p;
	p.type = P_NEW;

	p.add(clientID); //0

	p.add(static_cast<int>(crossDir)); //1
	p.add(offsetX); //2
	p.add(ratioY); //3

	p.add(params.name); //4
	p.add(params.pp.colorBegin.toInteger()); //5
	p.add(params.pp.colorEnd.toInteger()); //6

	return p;
}

Packet PacketCreator::P_Del(const EntityID clientID)
{
	Packet p;
	p.type = P_DEL;

	p.add(clientID);

	return p;
}

Packet PacketCreator::P_Name(const std::string& name)
{
	Packet p;
	p.type = P_NAME;

	p.add(name);

	return p;
}

Packet PacketCreator::P_ParticleParams(const ParticleParams& particleParams)
{
	Packet p;
	p.type = P_PARTICLE_PARAMS;

	p.add(particleParams.colorBegin.toInteger());
	p.add(particleParams.colorEnd.toInteger());

	return p;
}

Packet PacketCreator::P_Screen(const Screen* screen)
{
	Packet p;
	p.type = P_SCREEN;

	p.add(screen->size.x);
	p.add(screen->size.y);

	return p;
}

Packet PacketCreator::P_Move(const sf::Vector2i delta)
{
	Packet p;
	p.type = P_MOVE;

	p.add(delta.x);
	p.add(delta.y);

	return p;
}
