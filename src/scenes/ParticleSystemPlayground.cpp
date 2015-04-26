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

#include <iostream>
#include "../engine/AppWindow.h"
#include "../graphics/Fireball.h"

ParticleSystemPlayground::ParticleSystemPlayground(AppWindow &window) :
Scene(window, "Particle System Playground"), renderer(window, 1000)
{
	conn.setReceiveHandler(std::bind(&ParticleSystemPlayground::onReceive, this, std::placeholders::_1, std::placeholders::_2));

	myPS_label.text().setFont(*scene_log.text().getFont());
	myPS_label.text().setCharacterSize(15);
	myPS_label.text().setString("MyPS");
	myPS_label.text().setPosition(10, -10);

	myPS = new Fireball(window, view_main);
	myPS->add(myPS_label);

	bgm.openFromFile("D:/Dropbox/SFML-Game - backup/SFML-Game/Data/audio/gardenparty_mono.wav");
}

ParticleSystemPlayground::~ParticleSystemPlayground()
{
	delete myPS;
}

void ParticleSystemPlayground::onload()
{
	getWindow().setMouseCursorVisible(false);

	view_hud = view_main = getWindow().getCurrentView();

	bgm.setPosition(view_main.getCenter().x, view_main.getCenter().y, 0);
	bgm.setMinDistance(1000);
	bgm.setLoop(true);
	bgm.play();

	conn.start("localhost", 12345);

	Packet p;

	p.mType = MessageType::UPDATE_INFO;

	p.add("Teguh");

	p.add('0' + myPS->colorBegin.r);
	p.add('0' + myPS->colorBegin.g);
	p.add('0' + myPS->colorBegin.b);
	p.add('0' + myPS->colorBegin.a);

	p.add('0' + myPS->colorEnd.r);
	p.add('0' + myPS->colorEnd.g);
	p.add('0' + myPS->colorEnd.b);
	p.add('0' + myPS->colorEnd.a);

	p.add(0);
	p.add(0);

	conn.send(p);
}

void ParticleSystemPlayground::unload()
{
	getWindow().setMouseCursorVisible(true);

	bgm.stop();

	conn.stop();
}

void ParticleSystemPlayground::handleEvent(const sf::Event &event)
{
	Scene::handleEvent(event);

	static bool vSync = false, particleBuilder = false, music1Toggle = false;
	static float view_main_offset_value = 5, view_main_offset_max = 15;

	switch (event.type)
	{
	case sf::Event::MouseWheelMoved:
		view_main.zoom(1 - event.mouseWheel.delta * 0.0625f);
		break;

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

	case sf::Event::KeyReleased:
		switch (event.key.code)
		{
		case sf::Keyboard::BackSpace:
			getWindow().removeScene(this->getID());
			break;
		case sf::Keyboard::Space:
			randomizeParticleColors(myPS);
			break;
		case sf::Keyboard::Delete:
			myPS->clear();
			break;

		case sf::Keyboard::A:
		case sf::Keyboard::D:
			view_main_offset.x = 0;
			break;
		case sf::Keyboard::W:
		case sf::Keyboard::S:
			view_main_offset.y = 0;
			break;

		case sf::Keyboard::V:
			getWindow().setVerticalSyncEnabled(vSync = !vSync);
			break;
		case sf::Keyboard::P:
			particleBuilder = !particleBuilder;
			myPS->setBuilder(particleBuilder ? ParticleBuilders::pbPoint : ParticleBuilders::pbSprite);
			break;
		case sf::Keyboard::M:
			music1Toggle = !music1Toggle;
			if (music1Toggle) bgm.stop();
			else bgm.play();
			break;

		case sf::Keyboard::Return:
			// example:
			Packet p;
			p.mType = MessageType::UPDATE_POS;
			p.add<float>(myPS->emitterPos.x);
			p.add<float>(myPS->emitterPos.y);
			conn.send(p);
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
	myPS->emitterPos = getWindow().getMousePositionRelativeToWindowAndView(view_main);
	myPS->update(deltaTime);

	view_main.move(view_main_offset);

	sf::Listener::setPosition(view_main.getCenter().x, view_main.getCenter().y, -100);

	updateNetworkedParticleSystem();

	scene_log.text().setString(
		"FPS: " + std::to_string(getWindow().getFPS())
		+ "\n"
		+ "\nRenderer>"
		+ "\nDraw calls: " + std::to_string(renderer.getDrawCallCount())
		+ "\nSprites   : " + std::to_string(renderer.getSpriteCount())
		+ "\n"
		+ "\nParticles>"
		+ "\ntotal: " + std::to_string(ParticleSystem::TotalParticleCount)
		+ "\n" + myPS_label.text().getString() + " count   : " + std::to_string(myPS->getParticleCount())
		+ "\n" + myPS_label.text().getString() + " vertices: " + std::to_string(myPS->getVertexCount())
		);
}

void ParticleSystemPlayground::render()
{
	getWindow().clear();

	renderer.resetStats();

	getWindow().setView(view_main); ///////////////////////////////////

	renderer.begin();

	renderer.draw(myPS);

	renderer.end();

	getWindow().setView(view_hud); ////////////////////////////////////

	renderer.begin();

	renderer.draw(scene_log);

	renderer.end();

	getWindow().display();
}

void ParticleSystemPlayground::randomizeParticleColors(ParticleSystem* p)
{
	p->colorBegin = sf::Color(rand() & 255, rand() & 255, rand() & 255);
	p->colorEnd = sf::Color(rand() & 255, rand() & 255, rand() & 255);
}

void ParticleSystemPlayground::onReceive(const Packet& p, sf::TcpSocket& socket)
{
	switch (p.mType)
	{
	case MessageType::CLIENT_DISCONNECTED:
		for (NetworkedParticleSystem& nps : players)
		{
			if (nps.id == p.get<Client::ID>(0))
			{

			}
		}
		break;

	case MessageType::CROSS_SCREENS:
		break;
	}
}

void ParticleSystemPlayground::updateNetworkedParticleSystem()
{

}
