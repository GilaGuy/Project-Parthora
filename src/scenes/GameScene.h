#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <SFML/Audio.hpp>
#include "../engine/Scene.h"
#include "../core/Renderer.h"
#include "../net/client/Connection.h"
#include "../net/Screen.h"

struct Player;

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

	Packet lastPacket;
};

#endif /* GAMESCENE_H */
