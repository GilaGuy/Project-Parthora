#ifndef PS_FIREBALL_H
#define PS_FIREBALL_H

#include "../effect/ParticleSystem.h"

class Fireball : public ParticleSystem
{
public:
	Fireball();
	~Fireball();

	void updateParticle(const sf::Time &deltaTime, ParticleSystem::Particle &p) override;

private:
	sf::Texture texture;
};

#endif /* PS_FIREBALL_H */
