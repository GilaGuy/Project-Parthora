/**
 * Print Screener.
 *
 * @date       November 1, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      Print screen utility to capture a screenshot of the window and save it to an image file.
 */

#include "PrintScreen.h"

#include "AppWindow.h"

const std::string PrintScreen::DEFAULT_BASE_NAME = "ScreenShot";
const std::string PrintScreen::DEFAULT_FILE_EXTENSION = ".png";

PrintScreen::PrintScreen(AppWindow& window) :
	appWindow(window),
	baseName(DEFAULT_BASE_NAME),
	fileExtension(DEFAULT_FILE_EXTENSION)
{}

void PrintScreen::setBaseName(const std::string& name)
{
	baseName = name;
}

void PrintScreen::setFileExtension(const std::string& ext)
{
	fileExtension = ext;
}

bool PrintScreen::printScreen()
{
	unsigned int nameSuffixCount = 0;

	while (image.loadFromFile(baseName + std::to_string(nameSuffixCount) + fileExtension)) ++nameSuffixCount;

	image = appWindow.capture();
	return image.saveToFile(baseName + std::to_string(nameSuffixCount) + fileExtension);
}

PrintScreen::~PrintScreen()
{}
