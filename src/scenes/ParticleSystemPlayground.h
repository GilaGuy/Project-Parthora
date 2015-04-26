#ifndef PARTICLESYSTEMPLAYGROUND_H
#define PARTICLESYSTEMPLAYGROUND_H

#include <SFML/Audio.hpp>
#include "../core/Renderer.h"
#include "../core/object/SGO.h"
#include "../core/object/TGO.h"
#include "../engine/Scene.h"
#include "../effect/ParticleSystem.h"
#include "../net/client/Connection.h"

struct NetworkedParticleSystem
{
	Client::ID id;
	ParticleSystem* ps;
	TGO label;
	bool crossed;
};

class ParticleSystemPlayground : public Scene
{
public:
	ParticleSystemPlayground(AppWindow &window);
	~ParticleSystemPlayground();

	void onload() override;
	void unload() override;
	void handleEvent(const sf::Event &event) override;
	void update(const sf::Time &deltaTime) override;
	void render() override;

	void randomizeParticleColors(ParticleSystem* p);

	void onReceive(const Packet& p, sf::TcpSocket& socket);

	void updateNetworkedParticleSystem();

	void createClientInfoPacket(const NetworkedParticleSystem& nps, Packet& p);

	NetworkedParticleSystem& createNPS(Client::ID id, std::string name);

private:
	sf::View view_hud, view_main;
	Renderer renderer;
	Connection conn;

	NetworkedParticleSystem* myNPS;
	std::vector<NetworkedParticleSystem> players;

	sf::Music bgm;
};

#endif /* PARTICLESYSTEMPLAYGROUND_H */
