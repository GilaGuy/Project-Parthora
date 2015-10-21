#ifndef PARTICLESYSTEMPLAYGROUND_H
#define PARTICLESYSTEMPLAYGROUND_H

#include <SFML/Audio.hpp>
#include "../core/Renderer.h"
#include "../core/object/SGO.h"
#include "../core/object/TGO.h"
#include "../engine/Scene.h"
#include "../effect/ParticleSystem.h"
#include "../net/client/Connection.h"
#include "../entities/Player.h"

class GameScene : public Scene
{
public:
	GameScene(AppWindow &window);
	~GameScene();

	void onload() override;
	void unload() override;
	void updateViews() override;
	void handleEvent(const sf::Event &event) override;
	void update(const sf::Time &deltaTime) override;
	void render() override;

	void setControlParticle(bool arg);

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

	Screen myScreen;

	sf::Texture particleTexture;
	sf::Music bgm;
};

#endif /* PARTICLESYSTEMPLAYGROUND_H */
