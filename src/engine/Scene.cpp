/**
 * AppWindow Scene.
 *
 * @date       2013-??-??
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      Has the purpose of containing, using, updating and displaying resources
 *             as well as handling incoming system events.
 *             Once a Scene has been created, it can be added to an AppWindow so it can be used.
 */

#include "Scene.h"
#include "AppWindow.h"

#include <iostream>

Scene::ID Scene::SCENE_ID = 0;

Scene::Scene(AppWindow &window, const std::string &name) :
m_window(window),
m_id(++SCENE_ID),
m_sceneName(name)
{
	std::cout << "Constructed: " << "Scene" << "[" << getID() << "] \"" << getName() << "\"" << std::endl;

	scene_font.loadFromFile("Data/fonts/consolas.ttf");
	scene_log().setFont(scene_font);
	scene_log().setCharacterSize(13);
}

Scene::~Scene()
{
	std::cout << "Destructed: " << "Scene" << "[" << getID() << "] \"" << getName() << "\"" << std::endl;
}

const std::string& Scene::getName() const
{
	return m_sceneName;
}

void Scene::setName(const std::string &name)
{
	m_sceneName = name;
	m_window.updateTitle();
}

Scene::ID Scene::getID() const
{
	return m_id;
}

void Scene::onload()
{}

void Scene::unload()
{}

void Scene::handleEvent(const sf::Event &event)
{
	sf::Keyboard::Key k = event.key.code;

	switch (event.type)
	{
	case sf::Event::Closed:
		m_window.close();
		break;

	case sf::Event::KeyReleased:
		if (k == AppWindow::KEY_FULLSCREEN)
		{
			getWindow().toggleFullScreen();
			onload();
		}
		else if (k == AppWindow::KEY_SCREENSHOT)
		{
			// TODO
		}
		// else { std::cout << "Key pressed: " << k << std::endl;
		break;

	default:
		// std::cout << "Event type: " << event.type << std::endl;
		break;
	}
}

void Scene::update(const sf::Time &deltaTime)
{}

void Scene::render()
{
	getWindow().clear();
	getWindow().display();
}

AppWindow &Scene::getWindow() const
{
	return m_window;
}
