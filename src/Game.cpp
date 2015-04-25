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

#include "scenes/ParticleSystemPlayground.h"

int main()
{
	AppWindow window;

	window.setTimePerFrame(60);

	window.create
		(
		"SFML Game",
		800, 450, true
		//,sf::Style::Fullscreen
		);

	window.setScene(Scene::Create<ParticleSystemPlayground>(window), false);

	window.run();

	return 0;
}
