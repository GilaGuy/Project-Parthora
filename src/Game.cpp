/**
 * Project Parthora's client driver.
 *
 * @date       March 2, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      Creates the window and runs it with the appropriate scene.
 */

#include "Game.h"

#include <SFML/Graphics.hpp>
#include "engine/AppWindow.h"
#include "engine/Scene.h"

#include "scenes/ParticleSystemPlayground.h"

#include <iostream>

std::string GameSettings::serverIP;

int main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Please specify the IP address!" << std::endl;
		return EXIT_FAILURE;
	}

	GameSettings::serverIP = argv[1];

	AppWindow window;

	window.setTimePerFrame(60);

	window.create
		(
		"Project Parthora",
		800, 450, true
		//,sf::Style::Fullscreen
		);

	window.setScene(Scene::Create<ParticleSystemPlayground>(window), false);

	window.run();

	return EXIT_SUCCESS;
}
