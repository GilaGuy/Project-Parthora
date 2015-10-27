/**
 * Player.
 *
 * @date       October 21, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      This file contains the Player class which represents a player on the screen.
 *             It is composed of all of the parts that is included in a player.
 *             It has static methods to Create, Find and Remove players from the pool.
 */

#include "Player.h"
#include "../effect/ParticleSystem.h"

#include "Fireball.h"
#include <iostream>

std::vector<Player*> Player::players;

Player* Player::Create(ClientID id, std::string name, ParticleSystemType pst, const sf::Font& font)
{
	Player* newPlayer = nullptr;

	// check if player already exists
	for (Player* player : players)
	{
		if (player->id == id)
		{
			newPlayer = player;
			break;
		}
	}

	// create new player if they're not found
	if (!newPlayer)
	{
		std::cout << "Creating new player!" << std::endl;

		players.push_back(new Player());
		newPlayer = players.back();

		switch (pst)
		{
		case FIREBALL:
			newPlayer->ps = new Fireball();
			break;

		default:
			newPlayer->ps = new ParticleSystem();
		}

		newPlayer->ps->add(newPlayer->label);
	}

	newPlayer->id = id;

	newPlayer->label.text().setFont(font);
	newPlayer->label.text().setCharacterSize(15);
	newPlayer->label.text().setPosition(10, -10);

	newPlayer->setName(name);

	std::cout << "Player added! ID: " << id << std::endl;

	return newPlayer;
}

Player* Player::Find(ClientID id)
{
	for (Player* player : players)
	{
		if (player->id == id)
		{
			return player;
		}
	}

	return nullptr;
}

bool Player::Remove(ClientID id)
{
	for (std::vector<Player*>::iterator it = Player::players.begin(); it != Player::players.end();)
	{
		if ((*it)->id == id)
		{
			delete (*it)->ps;
			delete (*it);

			it = Player::players.erase(it);

			std::cout << "Player removed! ID: " << id << std::endl;
			return true;
		}
		else
		{
			++it;
		}
	}

	std::cout << "Player to remove not found! ID: " << id << std::endl;
	return false;
}

ClientParams Player::extractClientParams() const
{
	ClientParams cp;

	cp.name = label.text().getString().toAnsiString();
	cp.emitterPos = ps->emitterPos;
	cp.pp.colorBegin = ps->colorBegin;
	cp.pp.colorEnd = ps->colorEnd;

	return cp;
}

void Player::setName(std::string name)
{
	label.text().setString(name);
}
