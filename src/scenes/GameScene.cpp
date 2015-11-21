/**
 * Project Parthora's main game scene.
 *
 * @date       March 9, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes
 */

#include "GameScene.h"

#include "../GameSettings.h"
#include "../engine/AppWindow.h"
#include "../net/PacketCreator.h"
#include "../net/entities/Client.h"
#include "../net/entities/Screen.h"
#include "../effect/impl/Fireball.h"

#include <iostream>

using namespace std;

GameScene::GameScene(AppWindow &window) : Scene(window, "Game Scene")
, renderer(window, 1000)
, me(nullptr)
, myScreen(new Screen())
{
	bgm.openFromFile("Data/audio/gardenparty_mono.wav");
}

GameScene::~GameScene()
{
	unload();
	delete myScreen; myScreen = nullptr;
}

void GameScene::onload()
{
	// initialize views

	view_hud = view_main = getWindow().getCurrentView();

	// establish connection

	cout << "Connecting to..." << GameSettings::toString() << endl;

	if (!conn.start(GameSettings::serverIP, GameSettings::serverPort))
	{
		cerr << "Failed to connect to the server!" << endl;
	}

	// create my player
	me = players.add(Client::MYSELF, sf::IpAddress::getLocalAddress().toString(), Player::ParticleSystemType::FIREBALL, *scene_log.text().getFont());

	// uncontrol the particle
	setControlParticle(isControllingParticle = false);

	// center the player

	sf::Vector2i center = sf::Vector2i(view_main.getCenter());

	me->ps->emitterPos.x = center.x;
	me->ps->emitterPos.y = center.y;

	// update views
	updateViews();

	// make it as if we haven't connected yet to indicate when the server has talked back to us
	onDisconnect();

	// music settings

	sf::Listener::setPosition(center.x, center.y, -100);
	bgm.setPosition(center.x, center.y, 0);
	bgm.setMinDistance(1000);
	bgm.setLoop(true);
}

void GameScene::unload()
{
	getWindow().setMouseCursorVisible(true);

	players.clear();

	bgm.stop();

	conn.stop();
}

void GameScene::updateViews()
{
	view_hud = getWindow().getCurrentView();
	view_main.setSize(view_hud.getSize());
	view_main.setCenter(getWindow().getSize().x * 0.5f, getWindow().getSize().y * 0.5f);

	myScreen->size = getWindow().getSize();

	conn.send(PacketCreator::Get().PlayerInfo(Client::MYSELF, me->extractClientParams(), myScreen));

	if (isControllingParticle) getWindow().setMouseCursorVisible(false);
	else getWindow().setMouseCursorVisible(true);
}

void GameScene::handleEvent(const sf::Event& event)
{
	Scene::handleEvent(event);

	static bool vSync = false, particleBuilder = false, bgmToggle = false;

	switch (event.type)
	{
	case sf::Event::MouseMoved:
	{
		if (isControllingParticle)
		{
			sf::Vector2i delta = sf::Vector2i(getWindow().getMousePosition(view_main) - view_main.getCenter());
			getWindow().setMousePosition(sf::Vector2i(view_main.getCenter()));

			// send a position update packet to server if my player is available
			if (me)
			{
				if (delta.x == 0 && delta.y == 0)
				{
					//no movement
				}
				else
				{
					//cout << delta.x << ", " << delta.y << endl;
					conn.send(PacketCreator::Get().PlayerMove(delta));
				}
			}
		}
	}
	break;

	/*
	case sf::Event::MouseWheelMoved:
		view_main.zoom(1 - event.mouseWheel.delta * 0.0625f);
		break;
	*/

	case sf::Event::KeyReleased:
		switch (event.key.code)
		{
		case sf::Keyboard::BackSpace:
			//getWindow().removeScene(this->getID());
			break;
		case sf::Keyboard::Space:
			setControlParticle(isControllingParticle = !isControllingParticle);
			break;
		case sf::Keyboard::Delete:
			for (Player* player : players.getList())
			{
				player->ps->clear();
			}
			break;

		case sf::Keyboard::V:
			getWindow().setVerticalSyncEnabled(vSync = !vSync);
			break;
		case sf::Keyboard::P:
			if (me)
			{
				randomizeParticleColors(me);
			}
			break;
		case sf::Keyboard::M:
			bgmToggle = !bgmToggle;
			if (bgmToggle) bgm.play();
			else bgm.stop();
			break;
		}
		break;

	case sf::Event::Resized:
		updateViews();
		break;
	}
}

void GameScene::update(const sf::Time& deltaTime)
{
	ParticleSystem::TotalParticleCount = 0;

	while (conn.pollEvent(connEvent))
	{
		handleConnectionEvent(connEvent);
	}

	for (Player* player : players.getList())
	{
		player->ps->update(deltaTime);
	}

	std::string log;

	log = getWindow().getName() + " by " + "Melvin Loho"
		+ "\n"
		+ "\n"
		+ "[FPS]: " + std::to_string(getWindow().getFPS())
		+ "\n"
		+ "\n[RENDERER]"
		+ "\nDraw calls: " + std::to_string(renderer.getDrawCallCount())
		+ "\nSprites   : " + std::to_string(renderer.getSpriteCount())
		+ "\n"
		+ "\n[PARTICLES]"
		+ "\nTotal count: " + std::to_string(ParticleSystem::TotalParticleCount)
		+ "\n";

	for (Player* player : players.getList())
	{
		log += "\n>" + player->label.text().getString()
			+ "\n x: " + std::to_string(player->ps->emitterPos.x)
			+ "\n y: " + std::to_string(player->ps->emitterPos.y)
			//+ "\n c: " + std::to_string(player->ps->getParticleCount())
			+ "\n";
	}

	scene_log.text().setString(log);
}

void GameScene::render()
{
	getWindow().clear();

	renderer.resetStats();

	getWindow().setView(view_main); ///////////////////////////////////

	renderer.begin();

	for (Player* player : players.getList())
	{
		renderer.draw(player->ps);
	}

	renderer.end();

	getWindow().setView(view_hud); ////////////////////////////////////

	renderer.begin();

	renderer.draw(scene_log);

	renderer.end();

	getWindow().display();
}

void GameScene::setControlParticle(bool arg)
{
	if (arg)
	{
		getWindow().setMouseCursorVisible(false);

		getWindow().setMousePosition(view_main.getCenter(), view_main);
	}
	else
	{
		getWindow().setMouseCursorVisible(true);
	}
}

void GameScene::randomizeParticleColors(Player* player)
{
	ClientParams cp = player->extractClientParams();

	cp.pp.colorBegin = sf::Color(rand() & 255, rand() & 255, rand() & 255);
	cp.pp.colorEnd = sf::Color(rand() & 255, rand() & 255, rand() & 255);

	conn.send(PacketCreator::Get().PlayerInfo(Client::MYSELF, cp, myScreen));
}

// NETWORK METHODS

void GameScene::handleConnectionEvent(const Connection::Event& connEvent)
{
	switch (connEvent.type)
	{
	case Connection::Event::CONNECT:
		onConnect();
		break;

	case Connection::Event::PACKET:
		onReceive(connEvent.packet);
		break;

	case Connection::Event::DISCONNECT:
		onDisconnect();
		break;
	}
}

void GameScene::onConnect()
{
	cout << "Connected to the server!" << endl;
}

void GameScene::onReceive(const Packet& receivedPacket)
{
	switch (receivedPacket.type)
	{
	case PLAYER_INFO:
	{
		EntityID id = receivedPacket.get<EntityID>(0);
		Player* player = nullptr;

		if (id == Client::MYSELF) player = me;
		else
		{
			player = players.get(id);
			if (player == nullptr) return;
		}

		player->setName(receivedPacket.get(1));
		player->ps->colorBegin = sf::Color(receivedPacket.get<sf::Uint32>(2));
		player->ps->colorEnd = sf::Color(receivedPacket.get<sf::Uint32>(3));
	}
	break;

	case PLAYER_NEW:
	{
		Player* newPlayer = players.add(receivedPacket.get<EntityID>(0), receivedPacket.get(4), Player::ParticleSystemType::FIREBALL, *scene_log.text().getFont());
		if (newPlayer->id == Client::MYSELF) me = newPlayer;

		switch (static_cast<Cross>(receivedPacket.get<int>(1)))
		{
		case CROSS_LEFT:
			newPlayer->ps->emitterPos.x = getWindow().getSize().x + receivedPacket.get<float>(2);
			break;
		case CROSS_RIGHT:
			newPlayer->ps->emitterPos.x = 0 - receivedPacket.get<float>(2);
			break;
		}

		newPlayer->ps->emitterPos.y = receivedPacket.get<float>(3) * getWindow().getSize().y;
		newPlayer->ps->colorBegin = sf::Color(receivedPacket.get<sf::Uint32>(5));
		newPlayer->ps->colorEnd = sf::Color(receivedPacket.get<sf::Uint32>(6));
	}
	break;

	case PLAYER_DEL:
	{
		players.rem(receivedPacket.get<EntityID>(0));
	}
	break;

	case PLAYER_MOVE:
	{
		Player* player = players.get(receivedPacket.get<EntityID>(2));
		if (player)
		{
			player->ps->emitterPos.x += receivedPacket.get<float>(0);
			player->ps->emitterPos.y += receivedPacket.get<float>(1);
		}
	}
	break;
	}
}

void GameScene::onDisconnect()
{
	cout << "Lost connection to server!" << endl;

	me->setName("UNCONNECTED");
}
