#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <SFML/Graphics.hpp>
#include "Scene.h"

class Scene;

class AppWindow : public sf::RenderWindow
{
public:
	static const sf::Vector2i DEFAULT_RESOLUTION;
	static const sf::Keyboard::Key KEY_FULLSCREEN;
	static const sf::Keyboard::Key KEY_SCREENSHOT;

	AppWindow();
	~AppWindow();

	void create
		(
			const std::string &name,
			unsigned int width,
			unsigned int height,
			bool autoResolution,
			int wStyle = sf::Style::Default,
			const sf::ContextSettings& cS = sf::ContextSettings(),
			bool vSync = false
			);

	const std::string& getName() const;
	void setName(const std::string &name);

	void setScene(Scene::Ptr wS, bool replaceCurrent = true);
	void removeScene(Scene::ID id);

	int getFPS() const;
	sf::View getCurrentView() const;
	sf::Vector2i getDesktopCenter() const;

	bool isFullscreen() const;
	bool isVSync() const;
	bool isRunning() const;

	void setTimePerFrame(int fps);
	void setVerticalSyncEnabled(bool enabled);

	void updateTitle();
	void toggleFullScreen();
	void ignoreOneEvent(sf::Event::EventType eventType);

	sf::Vector2i getMousePosition() const;
	sf::Vector2f getMousePosition(const sf::View &view) const;

	// AppWindow's setMousePosition methods do not trigger an sf::Event as it will be automatically consumed.

	void setMousePosition(const sf::Vector2i &position);
	void setMousePosition(const sf::Vector2f & position, const sf::View & view);
	void setMousePositionAbsolute(const sf::Vector2i &position);

	void run();

private:
	sf::Vector2u m_lastWindowSize;
	int m_lastWindowStyle;
	bool m_autoResolution, m_isFullscreen, m_isVSync, m_isRunning;

	sf::Time m_elapsedTime, m_timePerFrame, m_timeSinceLastUpdate;

	std::vector<sf::Event::EventType> eventTypesToIgnoreOnce;

	std::string m_windowName;
	std::vector<Scene::Ptr> m_windowScenes;
	Scene::Ptr m_emptyScene;
};

#endif /* APPWINDOW_H */
