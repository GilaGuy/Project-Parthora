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

#include "ParticleSystemPlayground.h"

#include "../engine/AppWindow.h"
#include "../graphics/Fireball.h"

#include <iostream>

using namespace std;

void randomizeParticleColors(ParticleSystem* ps)
{
	ps->colorBegin = sf::Color(rand() & 255, rand() & 255, rand() & 255);
	ps->colorEnd = sf::Color(rand() & 255, rand() & 255, rand() & 255);
}

ParticleSystemPlayground::ParticleSystemPlayground(AppWindow &window) :
Scene(window, "Particle System Playground"), renderer(window, 1000), me(nullptr)
{
	conn.setReceiveHandler(std::bind(&ParticleSystemPlayground::onReceive, this, std::placeholders::_1, std::placeholders::_2));

	particleTexture.loadFromFile("Data/textures/particle_1.tga");
	particleTexture.setSmooth(true);

	bgm.openFromFile("D:/Dropbox/SFML-Game - backup/SFML-Game/Data/audio/gardenparty_mono.wav");
}

ParticleSystemPlayground::~ParticleSystemPlayground()
{
	unload();
}

void ParticleSystemPlayground::onload()
{
	view_hud = view_main = getWindow().getCurrentView();

	if (conn.start("localhost", 12345))
	{
		cout << "Connected to the server!" << endl;
	}
	else
	{
		cerr << "Failed to connect to the server!" << endl;
	}

	Packet p;

	createClientInfoPacket(
		createPlayer(Client::ID_MYSELF, "Teguh", new Fireball(getWindow(), view_main), particleTexture)
		, p
		);

	sf::Vector2f center = view_main.getCenter();

	setControlParticle(isControllingParticle = true, center);

	sf::Listener::setPosition(center.x, center.y, -100);
	bgm.setPosition(center.x, center.y, 0);
	bgm.setMinDistance(1000);
	bgm.setLoop(true);

	conn.send(p);
}

void ParticleSystemPlayground::unload()
{
	getWindow().setMouseCursorVisible(true);

	bgm.stop();

	for (Player& player : players)
	{
		delete player.ps;
	}
	players.clear();

	conn.stop();
}

void ParticleSystemPlayground::handleEvent(const sf::Event &event)
{
	Scene::handleEvent(event);

	static bool vSync = false, particleBuilder = false, music1Toggle = false;
	//static float view_main_offset_value = 5, view_main_offset_max = 15;

	switch (event.type)
	{
	case sf::Event::MouseMoved:
	{
		if (isControllingParticle)
		{
			sf::Vector2f delta = getWindow().getMousePositionRelativeToWindowAndView(view_main) - view_main.getCenter();
			sf::Mouse::setPosition(sf::Vector2i(view_main.getCenter()), getWindow());
			getWindow().pollEvent(sf::Event()); // <-- SFML hack: it prevents an infinite recursion by capturing the sf::Event generated by sf::Mouse::setPosition

			if (me == nullptr)
			{
				// send an UPDATE_POS packet to server

				Packet p;
				p.mType = MessageType::UPDATE_POS;

				p.add(Client::ID_MYSELF);
				p.add(delta.x);
				p.add(delta.y);

				conn.send(p);
			}
			else
			{
				me->ps->emitterPos += delta;
			}
		}
	}
	break;

	case sf::Event::MouseWheelMoved:
		view_main.zoom(1 - event.mouseWheel.delta * 0.0625f);
		break;

		/*
		case sf::Event::KeyPressed:
		switch (event.key.code)
		{
		case sf::Keyboard::A:
		if (view_main_offset.x > -view_main_offset_max)
		view_main_offset.x += -view_main_offset_value;
		break;
		case sf::Keyboard::D:
		if (view_main_offset.x < view_main_offset_max)
		view_main_offset.x += view_main_offset_value;
		break;
		case sf::Keyboard::W:
		if (view_main_offset.y > -view_main_offset_max)
		view_main_offset.y += -view_main_offset_value;
		break;
		case sf::Keyboard::S:
		if (view_main_offset.y < view_main_offset_max)
		view_main_offset.y += view_main_offset_value;
		break;
		}
		break;
		*/

	case sf::Event::KeyReleased:
		switch (event.key.code)
		{
		case sf::Keyboard::BackSpace:
			//getWindow().removeScene(this->getID());
			break;
		case sf::Keyboard::Space:
			setControlParticle(isControllingParticle = !isControllingParticle, view_main.getCenter());
			break;
		case sf::Keyboard::Delete:
			for (Player& player : players)
			{
				player.ps->clear();
			}
			break;

			/*
			case sf::Keyboard::A:
			case sf::Keyboard::D:
			view_main_offset.x = 0;
			break;
			case sf::Keyboard::W:
			case sf::Keyboard::S:
			view_main_offset.y = 0;
			break;
			*/

		case sf::Keyboard::V:
			getWindow().setVerticalSyncEnabled(vSync = !vSync);
			break;
		case sf::Keyboard::P:
			particleBuilder = !particleBuilder;
			for (Player& player : players)
			{
				player.ps->setBuilder(particleBuilder ? ParticleBuilders::pbPoint : ParticleBuilders::pbSprite);
			}
			break;
		case sf::Keyboard::M:
			music1Toggle = !music1Toggle;
			if (music1Toggle) bgm.stop();
			else bgm.play();
			break;
		}
		break;

	case sf::Event::Resized:
		view_hud = getWindow().getCurrentView();
		view_main.setSize(view_hud.getSize());
		view_main.setCenter(getWindow().getSize().x * 0.5f, getWindow().getSize().y * 0.5f);
		break;
	}
}

void ParticleSystemPlayground::update(const sf::Time &deltaTime)
{
	for (Player& player : players)
	{
		player.ps->update(deltaTime);
	}

	//view_main.move(view_main_offset);

	updatePlayers();

	scene_log.text().setString(
		"[FPS]: " + std::to_string(getWindow().getFPS())
		+ "\n"
		+ "\n[RENDERER]"
		+ "\nDraw calls: " + std::to_string(renderer.getDrawCallCount())
		+ "\nSprites   : " + std::to_string(renderer.getSpriteCount())
		+ "\n"
		+ "\n[PARTICLES]"
		+ "\nTotal: " + std::to_string(ParticleSystem::TotalParticleCount)
		);

	for (Player& player : players)
	{
		scene_log.text().setString(scene_log.text().getString() + "\n>" + player.label.text().getString() + ": " + std::to_string(player.ps->getParticleCount()));
	}
}

void ParticleSystemPlayground::render()
{
	getWindow().clear();

	renderer.resetStats();

	getWindow().setView(view_main); ///////////////////////////////////

	renderer.begin();

	for (Player& player : players)
	{
		renderer.draw(player.ps);
	}

	renderer.end();

	getWindow().setView(view_hud); ////////////////////////////////////

	renderer.begin();

	renderer.draw(scene_log);

	renderer.end();

	getWindow().display();
}

void ParticleSystemPlayground::setControlParticle(bool arg, sf::Vector2f syncLocation)
{
	if (arg)
	{
		getWindow().setMouseCursorVisible(false);
		sf::Mouse::setPosition(sf::Vector2i(syncLocation), getWindow());
		me->ps->emitterPos = syncLocation;
	}
	else
	{
		getWindow().setMouseCursorVisible(true);
	}
}

void ParticleSystemPlayground::createClientInfoPacket(const Player& player, Packet& p)
{
	p.mType = CLIENT_INFO;

	p.mParams.clear();

	p.add(player.id);

	p.add('0' + player.ps->colorBegin.r);
	p.add('0' + player.ps->colorBegin.g);
	p.add('0' + player.ps->colorBegin.b);
	p.add('0' + player.ps->colorBegin.a);

	p.add('0' + player.ps->colorEnd.r);
	p.add('0' + player.ps->colorEnd.g);
	p.add('0' + player.ps->colorEnd.b);
	p.add('0' + player.ps->colorEnd.a);
}

Player& ParticleSystemPlayground::createPlayer(Client::ID id, std::string name, ParticleSystem* ps, const sf::Texture& texture)
{
	Player* newPlayer = nullptr;

	// check if player already exists
	for (Player& p : players)
	{
		if (p.id == id)
		{
			newPlayer = &p;
			delete newPlayer->ps;
		}
	}

	// create new player if they're not found
	if (newPlayer == nullptr)
	{
		players.emplace_back(Player());

		newPlayer = &players.back();
	}

	newPlayer->id = id;

	newPlayer->ps = ps;
	newPlayer->ps->setTexture(texture);

	newPlayer->label.text().setFont(*scene_log.text().getFont());
	newPlayer->label.text().setCharacterSize(15);
	newPlayer->label.text().setPosition(10, -10);
	newPlayer->label.text().setString(name);

	newPlayer->crossingStat = NOT_CROSSED;

	newPlayer->ps->add(newPlayer->label);

	if (id == Client::ID_MYSELF)
	{
		me = newPlayer;
	}

	return *newPlayer;
}

void ParticleSystemPlayground::onReceive(const Packet& p, sf::TcpSocket& socket)
{
	cout << "recv> " << p.encode() << endl;

	switch (p.mType)
	{
	case CROSS_SCREENS:
	{
		Player& newPlayer = createPlayer(p.get<Client::ID>(0), p.get(1), new Fireball(getWindow(), view_main), particleTexture);
		CrossingDirection crossDir = static_cast<CrossingDirection>(p.get<int>(2));

		switch (crossDir)
		{
		case LEFT:
			newPlayer.ps->emitterPos.x = getWindow().getSize().x + p.get<float>(3);
			break;
		case RIGHT:
			newPlayer.ps->emitterPos.x = 0 - p.get<float>(3);
			break;
		}

		newPlayer.ps->emitterPos.y = p.get<float>(4) * getWindow().getSize().y;

		newPlayer.ps->colorBegin = sf::Color(p.get<sf::Uint16>(5), p.get<sf::Uint16>(6), p.get<sf::Uint16>(7), p.get<sf::Uint16>(8));
		newPlayer.ps->colorEnd = sf::Color(p.get<sf::Uint16>(9), p.get<sf::Uint16>(10), p.get<sf::Uint16>(11), p.get<sf::Uint16>(12));
	}
	break;

	case CROSS_STATUS:
	{
		Client::ID clientID = p.get<Client::ID>(0);

		for (Player& player : players)
		{
			if (player.id == clientID)
			{
				if (p.get<bool>(1))
				{
					player.crossingStat = CROSSED_GLOBAL;
				}
				else
				{
					player.crossingStat = NOT_CROSSED;
				}
				return;
			}
		}
	}
	break;

	case CLIENT_DISCONNECTED:
		for (std::vector<Player>::iterator it = players.begin(); it != players.end();)
		{
			if (it->id == p.get<Client::ID>(0))
			{
				delete it->ps;

				it = players.erase(it);
				return;
			}
		}
		break;
	}
}

void ParticleSystemPlayground::updatePlayers()
{
	float screenPaddingX = getWindow().getSize().x * 0.125f;
	float boundaryLeft = screenPaddingX, boundaryRight = getWindow().getSize().x - screenPaddingX;

	Packet p;
	p.mType = CROSS_SCREENS;

	for (Player& player : players)
	{
		switch (player.crossingStat)
		{
		case NOT_CROSSED:
		{
			CrossingDirection crossDir;
			float xOffset = 0;

			if (player.ps->emitterPos.x < boundaryLeft)
			{
				crossDir = LEFT;
				xOffset = 0 + player.ps->emitterPos.x;
			}
			else if (player.ps->emitterPos.x > boundaryRight)
			{
				crossDir = RIGHT;
				xOffset = getWindow().getSize().x - player.ps->emitterPos.x;
			}
			else
			{
				crossDir = NO_CD;
			}

			if (crossDir != NO_CD)
			{
				p.mParams.clear();
				p.add(player.id);
				p.add(player.label.text().getString().toAnsiString());
				p.add((int)crossDir);
				p.add(xOffset);
				p.add(player.ps->emitterPos.y / getWindow().getSize().y);

				conn.send(p);

				player.crossingStat = CROSSED_LOCAL;
			}
		}
		break;

		case CROSSED_LOCAL:
			// waiting for server to send back crossing success/fail.
			break;

		case CROSSED_GLOBAL:
			// check if it's no longer visible, if true, then send a REMOVE_TRACKING msg to the server and delete it.
			break;
		}
	}
}
