/**
* A fireball implementation of the Particle System.
*
* @date       March 2, 2015
*
* @revisions
*
* @designer   Melvin Loho
*
* @programmer Melvin Loho
*
* @notes
*/

#ifndef PS_FIREBALL_H
#define PS_FIREBALL_H

#include "../effect/ParticleSystem.h"

using namespace std;

class Fireball : public ParticleSystem
{
public:
	Fireball() : ParticleSystem(5000)
	{
		lifeTimeMin = sf::seconds(0.f), lifeTimeMax = sf::seconds(2.f);
		velMin = 3, velMax = 30, angleOffsetMin = -1.0f, angleOffsetMax = 1.0f;
		spawnRate = 15;
		colorBegin = sf::Color::Green;
		colorEnd = sf::Color(128, 240, 255);
		alphaMin = alphaMax = 128 / 10;

		texture.loadFromFile("Data/textures/particle_1.tga");
		texture.setSmooth(true);
		setTexture(texture);
	}

	~Fireball()
	{}

	void updateParticle(const sf::Time &deltaTime, ParticleSystem::Particle &p) override;

private:
	sf::Texture texture;
};

void Fireball::updateParticle(const sf::Time &deltaTime, ParticleSystem::Particle &p)
{
	p.lifetime -= deltaTime;

	if (isDead(p)) return;

	float currcalc_lifeTimeRatio = p.lifetime / lifeTimeMax;
	sf::Vector2f currcalc_deltaPos = emitterPos - p.pos;

	p.clr.r = static_cast<sf::Uint8>(colorEnd.r + currcalc_lifeTimeRatio * (colorBegin.r - colorEnd.r));
	p.clr.g = static_cast<sf::Uint8>(colorEnd.g + currcalc_lifeTimeRatio * (colorBegin.g - colorEnd.g));
	p.clr.b = static_cast<sf::Uint8>(colorEnd.b + currcalc_lifeTimeRatio * (colorBegin.b - colorEnd.b));
	p.clr.a = static_cast<sf::Uint8>(velMax / p.vel * alphaMax);

	if (abs(currcalc_deltaPos.x) > 0.f || abs(currcalc_deltaPos.y) > 0.f)
	{
		p.vel = p.vel * currcalc_lifeTimeRatio + p.vel * 0.15f;

		float currcalc_theta_rad = atan2(currcalc_deltaPos.y, currcalc_deltaPos.x);
		p.pos.x += p.vel * cos(currcalc_theta_rad + p.angleOffset * currcalc_lifeTimeRatio);
		p.pos.y += p.vel * sin(currcalc_theta_rad + p.angleOffset * currcalc_lifeTimeRatio);
	}
}

#endif /* PS_FIREBALL_H */