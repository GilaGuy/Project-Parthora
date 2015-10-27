#ifndef PLAYER_H
#define PLAYER_H

#include "../net/Protocol.h"
#include "../core/object/TGO.h"

class ParticleSystem;

struct Player
{
	enum ParticleSystemType
	{
		NONE,
		FIREBALL
	};

	static Player* Create(ClientID id, std::string name, ParticleSystemType pst, const sf::Font& font);
	static Player* Find(ClientID id);
	static bool Remove(ClientID id);

	ClientParams extractClientParams() const;
	void setName(std::string name);

	static std::vector<Player*> players;
	ClientID id;
	ParticleSystem* ps;
	TGO label;
};

#endif /* PLAYER_H */
