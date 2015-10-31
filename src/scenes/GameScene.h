#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <SFML/Audio.hpp>
#include "../engine/Scene.h"
#include "../core/Renderer.h"
#include "../net/client/Connection.h"
#include "../net/entities/Player.h"

struct Screen;

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

	void handleConnectionEvent(const Connection::Event &connEvent);
	void onConnect();
	void onReceive(const Packet& p);
	void onDisconnect();

private:
	sf::View view_hud, view_main;
	Renderer renderer;

	Connection conn;
	Connection::Event connEvent;

	bool isControllingParticle;
	PlayerManager players;
	Player* me;
	Screen* myScreen;

	sf::Music bgm;
};

#endif // GAMESCENE_H
