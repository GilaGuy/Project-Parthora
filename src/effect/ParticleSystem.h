#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "../core/object/BGO.h"

class ParticleBuilder;

class ParticleSystem : public BGO
{
	friend class ParticleBuilders;

public:
	struct Particle
	{
		sf::Vector2f pos;
		sf::Color clr;
		sf::Time lifetime;
		float vel;
		float angleOffset;
	};

	static unsigned int TotalParticleCount;

	ParticleSystem(unsigned int particleAmount = 5000);
	virtual ~ParticleSystem();

	sf::Transform getLocalTransform() const override;
	void draw(Renderer &renderer, sf::RenderStates states) const override;
	void update(const sf::Time &deltaTime) override;

	void setTexture(const sf::Texture &texture);
	void setBuilder(ParticleBuilder &builder);

	void clear();

	unsigned int getParticleCount() const;
	unsigned int getVertexCount() const;

	virtual bool isDead(const Particle &p) const;

	sf::Vector2f	emitterPos;
	unsigned int	spawnRate;
	sf::Color		colorBegin, colorEnd;
	sf::Uint8		alphaMin, alphaMax;
	sf::Time		lifeTimeMin, lifeTimeMax;
	float			velMin, velMax;
	float			angleOffsetMin, angleOffsetMax;

protected:
	virtual void updateParticle(const sf::Time &deltaTime, Particle &p);

private:
	const unsigned int m_particleCount;
	mutable unsigned int m_vertexCount;

	ParticleBuilder *m_builder;
	sf::PrimitiveType m_pType;
	const sf::Uint8 m_vCountMax;
	sf::Uint8 m_singleParticleVertexCount;

	Particle *m_particles;
	sf::Vertex *m_vertices;
	mutable bool dirtyVertices;

	const sf::Texture *m_texture;
	sf::Vector2f m_textureSize, m_spriteHalfSize;
};

class ParticleBuilder
{
public:
	virtual unsigned int build(const ParticleSystem &ps, const unsigned int &m_particleCount, const ParticleSystem::Particle *m_particles, sf::Vertex *m_vertices) = 0;
	virtual sf::PrimitiveType getType() const = 0;
	virtual sf::Uint8 getCount() const = 0;

protected:
	inline void setVertex(sf::Vertex& v, float xP, float yP, float xTC, float yTC, sf::Color c) const
	{
		v.position.x = xP;
		v.position.y = yP;
		v.texCoords.x = xTC;
		v.texCoords.y = yTC;
		v.color = c;
	}
};

class ParticleBuilders
{
public:
	static ParticleBuilder &pbSprite;
	static ParticleBuilder &pbPoint;

private:
	class PbSprite : public ParticleBuilder
	{
	public:
		unsigned int build(const ParticleSystem &ps, const unsigned int &m_particleCount, const ParticleSystem::Particle *m_particles, sf::Vertex *m_vertices);
		sf::PrimitiveType getType() const override;
		sf::Uint8 getCount() const override;
	};
	class PbPoint : public ParticleBuilder
	{
	public:
		unsigned int build(const ParticleSystem &ps, const unsigned int &m_particleCount, const ParticleSystem::Particle *m_particles, sf::Vertex *m_vertices);
		sf::PrimitiveType getType() const override;
		sf::Uint8 getCount() const override;
	};
};

#endif /* PARTICLESYSTEM_H */
