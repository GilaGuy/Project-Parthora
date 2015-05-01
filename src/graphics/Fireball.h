#ifndef PS_FIREBALL_H
#define PS_FIREBALL_H

#include <SFML/Audio.hpp>
#include "../effect/ParticleSystem.h"

class AppWindow;

class Fireball : public ParticleSystem
{
public:
	Fireball(AppWindow& window, sf::View& view);
	~Fireball();

	void update(const sf::Time& deltaTime) override;

protected:
	void updateParticle(const sf::Time &deltaTime, ParticleSystem::Particle &p) override;
	void draw(Renderer& renderer, sf::RenderStates states) const override;

private:
	AppWindow& window;
	sf::View& view;

	sf::Vector2f lastEmitterPos;

	sf::SoundBuffer sb;
	sf::Sound swoosh;
	float swooshVolume;
	int swooshStoppedLen;

	float wavePhase;
	sf::Vector2f waveAmp;

	sf::Shader shader_shake;
};

#endif /* PS_FIREBALL_H */
