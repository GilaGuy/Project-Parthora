/**
 * Application Window.
 *
 * @date       2013-??-??
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      Enhances the sf::RenderWindow with a plethora of helper functions.
 *             Manages and uses Scenes.
 *             Has its own game loop.
 */

#include "AppWindow.h"

#include <iostream>

const sf::Vector2i AppWindow::DEFAULT_RESOLUTION(1366, 768);
const sf::Keyboard::Key AppWindow::KEY_FULLSCREEN = sf::Keyboard::F11;
const sf::Keyboard::Key AppWindow::KEY_SCREENSHOT = sf::Keyboard::F12;

AppWindow::AppWindow() :
m_nf_size(), m_nf_wStyle(sf::Style::Default), m_autoRes(false),
m_emptyScene(Scene::Create<Scene>(*this)),
m_zoomDelta(0.05f),
m_isFullscreen(false),
m_isVSync(false),
m_isRunning(false)
{
	m_windowScenes.push_back(m_emptyScene);
}

AppWindow::~AppWindow()
{}

void AppWindow::create(
	const std::string &name,
	unsigned int width,
	unsigned int height,
	bool autoResolution,
	int wStyle,
	const sf::ContextSettings& cS,
	bool vSync)
{
	const sf::VideoMode &desktop = sf::VideoMode::getDesktopMode();

	m_isFullscreen = wStyle == sf::Style::Fullscreen;

	sf::Vector2u vmodeSize(
		m_isFullscreen ? (autoResolution ? desktop.width : width) : width,
		m_isFullscreen ? (autoResolution ? desktop.height : height) : height);

	sf::RenderWindow::create(sf::VideoMode(vmodeSize.x, vmodeSize.y, desktop.bitsPerPixel),
		name, wStyle, cS);

	setName(name);
	setPosition(getDesktopCenter());
	setVerticalSyncEnabled(vSync);

	if (!m_isFullscreen) m_nf_wStyle = wStyle;
	m_autoRes = autoResolution;
	m_isVSync = vSync;
}

const std::string& AppWindow::getName() const {
	return m_windowName;
}

void AppWindow::setName(const std::string &name) {
	m_windowName = name;
	updateTitle();
}

void AppWindow::setScene(Scene::Ptr wS, bool replaceCurrent) {
	m_windowScenes.back()->unload();

	if (replaceCurrent || m_windowScenes.back() == m_emptyScene) {
		m_windowScenes.back() = wS;
	}
	else {
		m_windowScenes.emplace_back(wS);
	}

	m_windowScenes.back()->onload();
	updateTitle();

	std::cout
		<< "Loaded: "
		<< "Scene \""
		<< m_windowScenes.back()->getName()
		<< "\"" << std::endl;
	std::cout << std::string(80, '-');
}

void AppWindow::removeScene(Scene::ID id) {
	if (m_windowScenes.back()->getID() == id) {
		m_windowScenes.back()->unload();
		if (m_windowScenes.size() == 1)
			m_windowScenes.back() = m_emptyScene;
		else
			m_windowScenes.pop_back();
	}
	else {
		for (int i = m_windowScenes.size() - 1; i-- > 0;) {
			if (m_windowScenes[i]->getID() == id) {
				m_windowScenes[i]->unload();
				m_windowScenes.erase(m_windowScenes.begin() + i);
				break;
			}
		}
	}

	m_windowScenes.back()->onload();
	updateTitle();

	clear();
	display();

	std::cout
		<< "Loaded: "
		<< "Scene \""
		<< m_windowScenes.back()->getName()
		<< "\"" << std::endl;
	std::cout << std::string(80, '-');
}

void AppWindow::updateTitle() {
	if (m_windowName.empty())
		setTitle("");
	else if (m_windowScenes.back()->getName().empty())
		setTitle(m_windowName);
	else
		setTitle(m_windowName + " - " + m_windowScenes.back()->getName());
}

void AppWindow::toggleFullScreen() {
	if (!m_isFullscreen) m_nf_size = getSize();
	m_isFullscreen = !m_isFullscreen;
	create(getName(), m_nf_size.x, m_nf_size.y, m_autoRes, m_isFullscreen ? sf::Style::Fullscreen : m_nf_wStyle, sf::ContextSettings(), m_isVSync);
}

void AppWindow::setTimePerFrame(int fps) {
	m_timePerFrame = sf::seconds(1.f / fps);
}

void AppWindow::setVerticalSyncEnabled(bool enabled) {
	sf::RenderWindow::setVerticalSyncEnabled(enabled);
	m_isVSync = enabled;
}

bool AppWindow::isFullscreen() const {
	return m_isFullscreen;
}

bool AppWindow::isVSync() const {
	return m_isVSync;
}

bool AppWindow::isRunning() const {
	return m_isRunning;
}

int AppWindow::getFPS() const {
	return (int)(1 / m_elapsedTime.asSeconds());
}

sf::Vector2i AppWindow::getDesktopCenter() const {
	const sf::VideoMode &desktop = sf::VideoMode::getDesktopMode();
	sf::Vector2u size = getSize();
	return sf::Vector2i(
		static_cast<int>(desktop.width * 0.5 - size.x * 0.5),
		static_cast<int>(desktop.height * 0.5 - size.y * 0.5));
}

sf::Vector2f AppWindow::getMousePositionRelativeToWindowAndView(const sf::View &view) const {
	return mapPixelToCoords(sf::Mouse::getPosition(*this), view);
}

sf::View AppWindow::getCurrentView() const {
	sf::Vector2u size = getSize();
	return sf::View(
		sf::Vector2f(size.x * 0.5f, size.y * 0.5f),
		sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)));
}

void AppWindow::run() {
	if (!m_isRunning)
	{
		m_isRunning = true;

		sf::Clock clock;
		sf::Event event;

		// LOOP
		while (isOpen())
		{
			// TIME UPDATES
			m_elapsedTime = clock.restart();
			m_timeSinceLastUpdate += m_elapsedTime;

			// CHECK FOR EVENTS
			while (pollEvent(event))
			{
				m_windowScenes.back()->handleEvent(event);
			}

			// TIME PER FRAME CONTROLLED LOOP
			while (m_timeSinceLastUpdate > m_timePerFrame)
			{
				m_timeSinceLastUpdate -= m_timePerFrame;

				for (Scene::Ptr s : m_windowScenes)
					s->update(m_timePerFrame);
			}

			// RENDER
			for (Scene::Ptr s : m_windowScenes)
				s->render();
		}

		m_isRunning = false;
	}
}
