/**
 * Player and its manager.
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
 *             Includes the PlayerManager to manage a set of players.
 */

#include "Player.h"
#include "../../effect/ParticleSystem.h"
#include "../../effect/impl/Fireball.h"

#include <iostream>

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

PlayerManager::PlayerManager()
{}

PlayerManager::~PlayerManager()
{
	clear();
}

Player* PlayerManager::add(Client::ID id, std::string name, Player::ParticleSystemType pst, const sf::Font& font)
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

		newPlayer = *players.insert(new Player()).first;

		switch (pst)
		{
		case Player::ParticleSystemType::FIREBALL:
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

Player* PlayerManager::get(Client::ID id)
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

bool PlayerManager::rem(Client::ID id)
{
	for (ListIter it = players.begin(); it != players.end();)
	{
		if ((*it)->id == id)
		{
			rem(it);
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

PlayerManager::ListIter PlayerManager::rem(ListIter it)
{
	Player* toRemove = *it;
	Client::ID id = toRemove->id;

	delete toRemove->ps; toRemove->ps = nullptr;
	delete toRemove; toRemove = nullptr;

	std::cout << "Player removed! ID: " << id << std::endl;

	return players.erase(it);
}

void PlayerManager::clear()
{
	for (ListIter it = players.begin(); it != players.end();) it = rem(it);

	players.clear();
}
