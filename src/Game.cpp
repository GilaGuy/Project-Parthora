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

#include <SFML/Graphics.hpp>
#include "engine/AppWindow.h"
#include "engine/Scene.h"
#include "GameSettings.h"

#include "scenes/ParticleSystemPlayground.h"

#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
	if (argc > 1)
	{
		GameSettings::serverIP = argv[1];

		if (argc > 2)
		{
			GameSettings::serverPort = stoul(argv[2]);
		}
	}

	cout << "[-Project Parthora-]" << endl;
	cout << "by Melvin Loho" << endl;
	cout << endl;

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
