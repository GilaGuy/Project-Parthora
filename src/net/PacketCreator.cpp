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

PacketCreator& PacketCreator::Get()
{
	static PacketCreator pc;
	return pc;
}

Packet PacketCreator::PlayerInfo(
	const sf::Vector2u windowSize,
	const ClientParams& params)
{
	Packet p;

	p.mType = PLAYER_INFO;

	p.add(params.name);

	p.add(windowSize.x);
	p.add(windowSize.y);

	p.add(params.ps.emitterPos.x);
	p.add(params.ps.emitterPos.y);

	p.add<sf::Uint32>(params.ps.colorBegin.r);
	p.add<sf::Uint32>(params.ps.colorBegin.g);
	p.add<sf::Uint32>(params.ps.colorBegin.b);
	p.add<sf::Uint32>(params.ps.colorBegin.a);

	p.add<sf::Uint32>(params.ps.colorEnd.r);
	p.add<sf::Uint32>(params.ps.colorEnd.g);
	p.add<sf::Uint32>(params.ps.colorEnd.b);
	p.add<sf::Uint32>(params.ps.colorEnd.a);

	return p;
}

Packet PacketCreator::PlayerPos(const sf::Vector2f pos)
{
	Packet p;

	p.mType = PLAYER_POS;

	p.add(pos.x);
	p.add(pos.y);

	return p;
}

Packet PacketCreator::PlayerNew(
	const ClientID clientID,
	const Cross crossDir,
	const float offsetX,
	const float ratioY,
	const ClientParams& params)
{
	Packet p;
	p.mType = PLAYER_NEW;

	p.add(clientID);
	p.add(static_cast<int>(crossDir));
	p.add(offsetX);
	p.add(ratioY);

	p.add(params.name);

	p.add<sf::Uint32>(params.ps.colorBegin.r);
	p.add<sf::Uint32>(params.ps.colorBegin.g);
	p.add<sf::Uint32>(params.ps.colorBegin.b);
	p.add<sf::Uint32>(params.ps.colorBegin.a);

	p.add<sf::Uint32>(params.ps.colorEnd.r);
	p.add<sf::Uint32>(params.ps.colorEnd.g);
	p.add<sf::Uint32>(params.ps.colorEnd.b);
	p.add<sf::Uint32>(params.ps.colorEnd.a);

	return p;
}

Packet PacketCreator::PlayerDel(const ClientID clientID)
{
	Packet p;

	p.mType = PLAYER_DEL;
	p.add(clientID);

	return p;
}
