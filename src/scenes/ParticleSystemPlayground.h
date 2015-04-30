#ifndef PARTICLESYSTEMPLAYGROUND_H
#define PARTICLESYSTEMPLAYGROUND_H

#include <SFML/Audio.hpp>
#include "../core/Renderer.h"
#include "../core/object/SGO.h"
#include "../core/object/TGO.h"
#include "../engine/Scene.h"
#include "../effect/ParticleSystem.h"
#include "../net/client/Connection.h"

struct Player
{
	Client::ID id;
	ParticleSystem* ps;
	TGO label;
};

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

	void randomizeParticleColors(Player* player);

	void setControlParticle(bool arg);

	void createPlayerInfoPacket(const Player* player, Packet& p);
	Player* createPlayer(Client::ID id, std::string name, ParticleSystem* ps, const sf::Texture& texture);

	void onReceive(const Packet& p);

private:
	sf::View view_hud, view_main;
	Renderer renderer;
	Connection conn;

	bool isControllingParticle;
	sf::Vector2f lastPos;

	Player* me;
	std::vector<Player*> players;

	sf::Texture particleTexture;
	sf::Music bgm;
};

#endif /* PARTICLESYSTEMPLAYGROUND_H */
