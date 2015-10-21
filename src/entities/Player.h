#ifndef PLAYER_H
#define PLAYER_H

#include "../net/Protocol.h"
#include "../effect/ParticleSystem.h"
#include "../core/object/TGO.h"

struct Player
{
	static Player* Create(ClientID id, std::string name, ParticleSystem* ps, const sf::Texture& texture, const sf::Font& font);
	static Player* Find(ClientID id);
	static bool Remove(ClientID id);

	ClientParams extractClientparams() const;
	void setName(std::string name);

	static std::vector<Player*> players;
	ClientID id;
	ParticleSystem* ps;
	TGO label;
};

#endif /* PLAYER_H */
