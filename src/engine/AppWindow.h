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

	void updateTitle();

	void toggleFullScreen();

	void setTimePerFrame(int fps);
	void setVerticalSyncEnabled(bool enabled);

	bool isFullscreen() const;
	bool isVSync() const;
	bool isRunning() const;

	int getFPS() const;
	sf::Vector2i getDesktopCenter() const;
	sf::Vector2f getMousePositionRelativeToWindowAndView(const sf::View &view) const;
	sf::View getCurrentView() const;

	void run();

private:
	sf::Vector2u m_nf_size;
	int m_nf_wStyle;
	bool m_autoRes;

	sf::Time m_elapsedTime, m_timePerFrame, m_timeSinceLastUpdate;

	std::string m_windowName;
	std::vector<Scene::Ptr> m_windowScenes;
	Scene::Ptr m_emptyScene;

	float m_zoomDelta;
	bool m_isFullscreen, m_isVSync, m_isRunning;
};

#endif /* APPWINDOW_H */
