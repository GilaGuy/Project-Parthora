#ifndef PLAYER_H
#define PLAYER_H

#include <set>
#include "../Protocol.h"
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

	ClientID id;
	ParticleSystem* ps;
	TGO label;
};

class PlayerManager
{
	typedef std::set<Player*> PlayerList;
	typedef PlayerList::iterator PlayerListIter;

public:
	PlayerManager();
	~PlayerManager();

	Player* add(ClientID id, std::string name, Player::ParticleSystemType pst, const sf::Font& font);
	Player* get(ClientID id);
	inline PlayerList getList() { return players; }
	bool rem(ClientID id);
	void clear();

private:
	PlayerList players;
};

#endif /* PLAYER_H */
