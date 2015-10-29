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
	const ClientID clientID,
	const ClientParams& params,
	const Screen& playerScreen)
{
	Packet p;
	p.type = PLAYER_INFO;

	p.add(clientID); //0

	p.add(params.name); //1
	p.add(params.pp.colorBegin.toInteger()); //2
	p.add(params.pp.colorEnd.toInteger()); //3

	p.add(playerScreen.size.x); //4
	p.add(playerScreen.size.y); //5

	return p;
}

Packet PacketCreator::PlayerMove(const sf::Vector2i delta)
{
	Packet p;
	p.type = PLAYER_MOVE;

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
	p.type = PLAYER_NEW;

	p.add(clientID); //0

	p.add(static_cast<int>(crossDir)); //1
	p.add(offsetX); //2
	p.add(ratioY); //3

	p.add(params.name); //4
	p.add(params.pp.colorBegin.toInteger()); //5
	p.add(params.pp.colorEnd.toInteger()); //6

	return p;
}

Packet PacketCreator::PlayerDel(const ClientID clientID)
{
	Packet p;
	p.type = PLAYER_DEL;

	p.add(clientID);

	return p;
}
