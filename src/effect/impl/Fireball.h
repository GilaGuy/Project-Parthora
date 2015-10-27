#ifndef PS_FIREBALL_H
#define PS_FIREBALL_H

#include <SFML/Audio.hpp>
#include "../ParticleSystem.h"

class Fireball : public ParticleSystem
{
public:
	Fireball();
	~Fireball();

	void update(const sf::Time& deltaTime) override;

protected:
	void updateParticle(const sf::Time &deltaTime, ParticleSystem::Particle &p) override;
	void draw(Renderer& renderer, sf::RenderStates states) const override;

private:
	// static resources>

	static bool staticResourceLoadLock;
	static sf::SoundBuffer sb;
	static sf::Texture particleTexture;

	// <static resources

	sf::Vector2f lastEmitterPos;

	float swooshVolume;
	int swooshStoppedLen;

	float wavePhase;
	sf::Vector2f waveAmp;

	sf::Sound swoosh;
	sf::Shader shader_shake;
};

#endif /* PS_FIREBALL_H */
