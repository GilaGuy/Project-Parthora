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
	CrossingStatus crossingStat;
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

	void onReceive(const Packet& p, sf::TcpSocket& socket);

	void createClientInfoPacket(const Player& player, Packet& p);
	Player& createPlayer(Client::ID id, std::string name, ParticleSystem* ps, const sf::Texture& texture);

	void updatePlayers();

private:
	sf::View view_hud, view_main;
	Renderer renderer;
	Connection conn;

	Player* me;
	std::vector<Player> players;

	sf::Texture particleTexture;
	sf::Music bgm;
};

#endif /* PARTICLESYSTEMPLAYGROUND_H */
