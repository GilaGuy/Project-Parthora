#ifndef PRINTSCREEN_H
#define PRINTSCREEN_H

#include <string>
#include <SFML/Graphics/Image.hpp>

class AppWindow;

class PrintScreen
{
public:
	static const std::string DEFAULT_BASE_NAME;
	static const std::string DEFAULT_FILE_EXTENSION;

	PrintScreen(AppWindow& window);
	~PrintScreen();

	void setBaseName(const std::string& name);
	void setFileExtension(const std::string& ext);
	bool printScreen();

private:
	AppWindow& appWindow;
	std::string baseName, fileExtension;
	sf::Image image;
};

#endif // PRINTSCREEN_H
