/**
* A fireball implementation of the Particle System.
*
* @date       March 2, 2015
*
* @revisions
*
* @designer   Melvin Loho
*
* @programmer Melvin Loho
*
* @notes
*/

#include "Fireball.h"
#include "../engine/AppWindow.h"
#include "../core/Renderer.h"

#include <iostream>

//TODO: make all the loadFromFile's done only once (make them also static variables).

Fireball::Fireball(AppWindow& window, sf::View& view) : ParticleSystem(5000),
window(window),
view(view),
lastEmitterPos(),
swooshVolume(),
swooshStoppedLen(),
wavePhase(),
waveAmp()
{
	lifeTimeMin = sf::seconds(0.f), lifeTimeMax = sf::seconds(2.f);
	velMin = 3, velMax = 30, angleOffsetMin = -1.0f, angleOffsetMax = 1.0f;
	spawnRate = 15;
	colorBegin = sf::Color::Green;
	colorEnd = sf::Color(128, 240, 255);
	alphaMin = alphaMax = 128 / 10;

	texture.loadFromFile("Data/textures/particle_1.tga");
	texture.setSmooth(true);
	setTexture(texture);

	sb.loadFromFile("Data/audio/SWOOSH_loop.wav");
	swoosh.setBuffer(sb);
	swoosh.setLoop(true);
	swoosh.setVolume(0);
	swoosh.play();

	shader_shake.loadFromFile("D:/Repositories/term-project/Source/Assets/shaders/wave.vert", sf::Shader::Vertex);
}

Fireball::~Fireball()
{
	swoosh.stop();
}

void Fireball::update(const sf::Time& deltaTime)
{
	ParticleSystem::update(deltaTime);

	sf::Vector2f delta(abs(emitterPos.x - lastEmitterPos.x), abs(emitterPos.y - lastEmitterPos.y));
	float magnitude = sqrt(delta.x * delta.x + delta.y * delta.y);

	if (magnitude > 5.0f)
	{
		swooshVolume += magnitude > 2.0f ? 2.0f : magnitude;
		swooshStoppedLen = 0;
	}
	else
	{
		swooshVolume -= 2.0f;
		swooshStoppedLen++;
	}

	swooshVolume = swooshVolume < 0.f ? 0.f : swooshVolume;
	swooshVolume = swooshVolume > 100.f ? 100.f : swooshVolume;

	if (swooshStoppedLen > 69)
	{
		swooshStoppedLen = 0;
		wavePhase = 0;
	}

	swoosh.setPitch(swooshVolume / 100);
	swoosh.setVolume(swooshVolume);

	++wavePhase;
	magnitude = std::min(magnitude / 5, 15.f);
	waveAmp.x = waveAmp.y = magnitude;

	shader_shake.setParameter("wave_phase", wavePhase);
	shader_shake.setParameter("wave_amplitude", waveAmp);

	lastEmitterPos = emitterPos;
}

void Fireball::updateParticle(const sf::Time &deltaTime, ParticleSystem::Particle &p)
{
	p.lifetime -= deltaTime;

	if (isDead(p)) return;

	float currcalc_lifeTimeRatio = p.lifetime / lifeTimeMax;
	sf::Vector2f currcalc_deltaPos = emitterPos - p.pos;

	p.clr.r = static_cast<sf::Uint8>(colorEnd.r + currcalc_lifeTimeRatio * (colorBegin.r - colorEnd.r));
	p.clr.g = static_cast<sf::Uint8>(colorEnd.g + currcalc_lifeTimeRatio * (colorBegin.g - colorEnd.g));
	p.clr.b = static_cast<sf::Uint8>(colorEnd.b + currcalc_lifeTimeRatio * (colorBegin.b - colorEnd.b));
	p.clr.a = static_cast<sf::Uint8>(velMax / p.vel * alphaMax);

	if (abs(currcalc_deltaPos.x) > 0.f || abs(currcalc_deltaPos.y) > 0.f)
	{
		p.vel = p.vel * currcalc_lifeTimeRatio + p.vel * 0.15f;

		float currcalc_theta_rad = atan2(currcalc_deltaPos.y, currcalc_deltaPos.x);
		p.pos.x += p.vel * cos(currcalc_theta_rad + p.angleOffset * currcalc_lifeTimeRatio);
		p.pos.y += p.vel * sin(currcalc_theta_rad + p.angleOffset * currcalc_lifeTimeRatio);
	}
}

void Fireball::draw(Renderer& renderer, sf::RenderStates states) const
{
	states.shader = &shader_shake;
	states.blendMode = sf::BlendAdd;

	ParticleSystem::draw(renderer, states);
}
