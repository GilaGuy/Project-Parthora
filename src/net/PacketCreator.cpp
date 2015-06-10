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
	const ClientParams& playerParams,
	const Screen& playerScreen)
{
	Packet p;

	p.mType = PLAYER_INFO;

	p.add(playerParams.name);

	p.add(playerScreen.size.x);
	p.add(playerScreen.size.y);

	p.add(playerParams.ps.emitterPos.x);
	p.add(playerParams.ps.emitterPos.y);

	p.add<sf::Uint32>(playerParams.ps.colorBegin.r);
	p.add<sf::Uint32>(playerParams.ps.colorBegin.g);
	p.add<sf::Uint32>(playerParams.ps.colorBegin.b);
	p.add<sf::Uint32>(playerParams.ps.colorBegin.a);

	p.add<sf::Uint32>(playerParams.ps.colorEnd.r);
	p.add<sf::Uint32>(playerParams.ps.colorEnd.g);
	p.add<sf::Uint32>(playerParams.ps.colorEnd.b);
	p.add<sf::Uint32>(playerParams.ps.colorEnd.a);

	return p;
}

Packet PacketCreator::PlayerMove(const sf::Vector2f delta)
{
	Packet p;

	p.mType = PLAYER_MOVE;

	p.add(delta.x);
	p.add(delta.y);

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
