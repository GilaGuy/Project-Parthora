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
	ClientID id;
	ClientParams params;
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
	void updateViews() override;
	void handleEvent(const sf::Event &event) override;
	void update(const sf::Time &deltaTime) override;
	void render() override;

	void setControlParticle(bool arg);

	Player* createPlayer(ClientID id, std::string name, ParticleSystem* ps, const sf::Texture& texture);
	void setPlayerName(Player* player, std::string name);
	void syncPPwithPS(ParticleParams& pp, const ParticleSystem& ps);

	void randomizeParticleColors(Player* player);

	void onConnect();
	void onReceive(const Packet& p);
	void onDisconnect();

private:
	sf::View view_hud, view_main;
	Renderer renderer;
	Connection conn;

	bool isControllingParticle;

	Player* me;
	std::vector<Player*> players;

	Screen myScreen;

	sf::Texture particleTexture;
	sf::Music bgm;
};

#endif /* PARTICLESYSTEMPLAYGROUND_H */
