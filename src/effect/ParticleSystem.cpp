/**
 * Particle System.
 *
 * @date       2014-??-??
 *
 * @revisions  2014-??-??
 *             Enable customization of particle builders.
 *
 *             2015-??-??
 *             Improved performance by changing when the particles are built.
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      A flexible particle system that can be customized in terms of:
 *             - what determines the death of a particle
 *             - how a particle is updated over time
 *             - how a particle is built so that it can be displayed on the screen
 *
 *             Currently holds a number of variables to affect each particle.
 *
 *             Upcoming features include customization of the particle itself:
 *             - what makes up a particle
 *             - how is a particle created
 */

#include "ParticleSystem.h"

#include <math.h>
#include "../core/Renderer.h"

unsigned int ParticleSystem::TotalParticleCount = 0;

ParticleSystem::ParticleSystem(unsigned int particleAmount) :
emitterPos(0.f, 0.f),
spawnRate(1),
colorBegin(sf::Color::White), colorEnd(sf::Color::White),
alphaMin(255), alphaMax(255),
lifeTimeMin(sf::seconds(1.f)), lifeTimeMax(sf::seconds(1.f)),
velMin(1.f), velMax(1.f),
angleOffsetMin(0.f), angleOffsetMax(0.f),

m_particleCount(particleAmount), m_vertexCount(0),
m_builder(&ParticleBuilders::pbSprite),
m_pType(m_builder->getType()),
m_vCountMax(m_builder->getCount()),
m_singleParticleVertexCount(m_vCountMax),
m_particles(new Particle[m_particleCount]),
m_vertices(new sf::Vertex[m_particleCount * SPRITE_VERTICES]),
dirtyVertices(true)
{
	for (unsigned int p = 0; p < m_particleCount; ++p)
	{
		m_particles[p] = Particle();
	}
	for (unsigned int p = 0; p < m_particleCount * SPRITE_VERTICES; ++p)
	{
		m_vertices[p] = sf::Vertex();
	}
}

ParticleSystem::~ParticleSystem()
{
	delete[] m_particles;
	delete[] m_vertices;
}

sf::Transform ParticleSystem::getLocalTransform() const
{
	static sf::Transform localtrans = sf::Transform::Identity;

	localtrans = sf::Transform::Identity;
	localtrans.translate(emitterPos);

	return localtrans;
}

void ParticleSystem::draw(Renderer &renderer, sf::RenderStates states) const
{
	if (dirtyVertices)
	{
		dirtyVertices = false;
		m_vertexCount = m_builder->build(*this, m_particleCount, m_particles, m_vertices);
	}

	if (m_pType == sf::PrimitiveType::Points)
		states.texture = nullptr;
	else
		states.texture = m_texture;

	renderer.sf_draw(m_vertices, m_vertexCount, m_pType, states);
}

void ParticleSystem::update(const sf::Time &deltaTime)
{
	unsigned int p = 0, spawnCount = 0;

	// SPAWN & UPDATE PARTICLES

	for (; p < m_particleCount; ++p)
	{
		if (spawnCount >= spawnRate) break;

		if (isDead(m_particles[p]))
		{
			++spawnCount;
			m_particles[p].pos = emitterPos;
			m_particles[p].clr = colorBegin;
			m_particles[p].lifetime = lifeTimeMin + sf::microseconds(rand()) / (sf::microseconds(RAND_MAX) / (lifeTimeMax - lifeTimeMin));
			m_particles[p].vel = velMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (velMax - velMin)));
			m_particles[p].angleOffset = angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffsetMax - angleOffsetMin)));
		}

		updateParticle(deltaTime, m_particles[p]);
	}

	// UPDATE REMAINING PARTICLES

	for (; p < m_particleCount; ++p)
	{
		updateParticle(deltaTime, m_particles[p]);
	}

	// UPDATE STATS

	dirtyVertices = true;
	TotalParticleCount += m_vertexCount / m_singleParticleVertexCount;
}

void ParticleSystem::setTexture(const sf::Texture &texture)
{
	m_texture = &texture;
	m_textureSize = static_cast <sf::Vector2f> (m_texture->getSize());
	m_spriteHalfSize = m_textureSize * 0.5f;
}

void ParticleSystem::setBuilder(ParticleBuilder &builder)
{
	if (builder.getCount() > m_vCountMax) throw "Particle system can't hold all these vertices!!"; //Solution: Change m_vertices into a vector...
	m_builder = &builder;
	m_pType = builder.getType();
	m_singleParticleVertexCount = builder.getCount();
}

void ParticleSystem::clear()
{
	for (unsigned int p = 0; p < m_particleCount; ++p)
	{
		m_particles[p].lifetime = sf::Time::Zero;
	}
}

unsigned int ParticleSystem::getParticleCount() const
{
	return m_vertexCount / m_singleParticleVertexCount;
}

unsigned int ParticleSystem::getVertexCount() const
{
	return m_vertexCount;
}

void ParticleSystem::updateParticle(const sf::Time &deltaTime, Particle &p)
{}

bool ParticleSystem::isDead(const Particle &p) const
{
	return (p.lifetime <= sf::Time::Zero);
}

unsigned int ParticleBuilders::PbSprite::build(const ParticleSystem &ps, const unsigned int &m_particleCount, const ParticleSystem::Particle *m_particles, sf::Vertex *m_vertices)
{
	unsigned int m_vertexCount = 0;

	for (unsigned int p = 0; p < m_particleCount; ++p)
	{
		if (!ps.isDead(m_particles[p]))
		{
			const sf::Vector2f& pos = m_particles[p].pos;
			const sf::Color& color = m_particles[p].clr;
			const sf::Vector2f& size = (m_particles[p].lifetime / ps.lifeTimeMax) * ps.m_spriteHalfSize;

			/* 0 */ setVertex(m_vertices[m_vertexCount++], pos.x - size.x, pos.y - size.y, 0.f, 0.f, color);
			/* 1 */ setVertex(m_vertices[m_vertexCount++], pos.x - size.x, pos.y + size.y, 0.f, ps.m_textureSize.y, color);
			/* 2 */ setVertex(m_vertices[m_vertexCount++], pos.x + size.x, pos.y - size.y, ps.m_textureSize.x, 0.f, color);

			/* 1 */ m_vertices[m_vertexCount++] = m_vertices[m_vertexCount - 2];
			/* 2 */ m_vertices[m_vertexCount++] = m_vertices[m_vertexCount - 2];
			/* 3 */ setVertex(m_vertices[m_vertexCount++], pos.x + size.x, pos.y + size.y, ps.m_textureSize.x, ps.m_textureSize.y, color);
		}
	}

	return m_vertexCount;
}
sf::PrimitiveType ParticleBuilders::PbSprite::getType() const
{
	return sf::PrimitiveType::Triangles;
}
sf::Uint8 ParticleBuilders::PbSprite::getCount() const
{
	return SPRITE_VERTICES;
}

unsigned int ParticleBuilders::PbPoint::build(const ParticleSystem &ps, const unsigned int &m_particleCount, const ParticleSystem::Particle *m_particles, sf::Vertex *m_vertices)
{
	unsigned int m_vertexCount = 0;

	for (unsigned int p = 0; p < m_particleCount; ++p)
	{
		if (!ps.isDead(m_particles[p]))
		{
			setVertex(m_vertices[m_vertexCount++], m_particles[p].pos.x, m_particles[p].pos.y, 0.f, 0.f, m_particles[p].clr);
		}
	}

	return m_vertexCount;
}
sf::PrimitiveType ParticleBuilders::PbPoint::getType() const
{
	return sf::PrimitiveType::Points;
}
sf::Uint8 ParticleBuilders::PbPoint::getCount() const
{
	return 1;
}

ParticleBuilder &ParticleBuilders::pbSprite = *(new PbSprite());
ParticleBuilder &ParticleBuilders::pbPoint = *(new PbPoint());
