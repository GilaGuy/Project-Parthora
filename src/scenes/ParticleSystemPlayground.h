#ifndef PARTICLESYSTEMPLAYGROUND_H
#define PARTICLESYSTEMPLAYGROUND_H

#include <SFML/Audio.hpp>

#include "../core/Renderer.h"
#include "../core/object/SGO.h"
#include "../core/object/TGO.h"
#include "../engine/Scene.h"
#include "../effect/ParticleSystem.h"
#include "../net/client/Connection.h"

class ParticleSystemPlayground : public Scene
{
public:
	ParticleSystemPlayground(AppWindow &window);
	~ParticleSystemPlayground();

	void onload() override;
	void unload() override;
	void handleEvent(const sf::Event &event) override;
	void update(const sf::Time &deltaTime) override;
	void render() override;

	void randomizeParticleColors(ParticleSystem* p);

	void onReceive(const Packet& p, sf::TcpSocket& socket);

	void updateNetworkParticles();

private:
	sf::View view_hud, view_main;

	Renderer renderer;

	sf::Shader shader_shake, shader_shockwave;

	ParticleSystem* ps_1;
	TGO ps_1_label;

	sf::Texture tex_1;
	SGO thing_1, thing_2;

	sf::Vector2f lastEmitterPos;

	sf::Music music1;

	sf::SoundBuffer sb;
	sf::Sound swoosh;
	float swooshVolume;
	int swooshStoppedLen;

	sf::Vector2f view_main_offset;

	Connection conn;
};

#endif /* PARTICLESYSTEMPLAYGROUND_H */
