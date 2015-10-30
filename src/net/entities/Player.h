#ifndef PLAYER_H
#define PLAYER_H

#include <set>
#include "../Shared.h"
#include "../entities/Client.h"
#include "../../core/object/TGO.h"

class ParticleSystem;

struct Player
{
	enum ParticleSystemType
	{
		NONE,
		FIREBALL
	};

	ClientParams extractClientParams() const;
	void setName(std::string name);

	Client::ID id;
	ParticleSystem* ps;
	TGO label;
};

class PlayerManager
{
public:
	typedef std::set<Player*> List;
	typedef List::iterator ListIter;

	PlayerManager();
	~PlayerManager();

	Player* add(Client::ID id, std::string name, Player::ParticleSystemType pst, const sf::Font& font);
	Player* get(Client::ID id);
	inline List& getList() { return players; }
	bool rem(Client::ID id);
	ListIter rem(ListIter it);
	void clear();

private:
	List players;
};

#endif // PLAYER_H
