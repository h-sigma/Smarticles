#include "ParticleSystem.hpp"
#include <random>
#include <iostream>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace
{
auto Table = initializeParticleData();

auto makeRandomEngine()
{
    return std::default_random_engine(std::random_device()());
}

auto engine = makeRandomEngine();

} // namespace

int getInt(int a, int b)
{
    std::uniform_int_distribution<> dist(a, b);
    return dist(engine);
}

ParticleSystem::ParticleSystem(Particle::Type type, sf::Texture &texture)
    : mTexture(texture), mType(type), mVertexArray(sf::Quads)
{
}

void ParticleSystem::addPath(sf::Vector2f point)
{
    mPath.push_back(point);
}

void ParticleSystem::addPath(float x, float y)
{
    mPath.emplace_back(x, y);
}

void ParticleSystem::addAffector(std::function<void(std::deque<Particle> &)> affector)
{
    mAffector.push_back(affector);
}

void ParticleSystem::addParticle(sf::Vector2f position)
{
    Particle particle = Table[mType];
    particle.radius.x = getInt(1, 1000);
    particle.position = position;
    mParticles.push_back(particle);
}

void ParticleSystem::computeVertices() const
{
    static int diff = 0;

    sf::Vector2f size(mTexture.getSize());
    sf::Vector2f half = size / 2.f;

    mVertexArray.clear();

    for (const auto &particle : mParticles)
    {
        sf::Color c = particle.color;
        sf::Vector2f pos = mPath[particle.pathindex];
        pos += particle.radius;
        const float ratio = particle.lifetime.asSeconds() / Table[mType].lifetime.asSeconds();
        c.a = static_cast<uint8_t>(255 * std::max(0.0f, ratio)); //can't forget to keep the alpha value positive

        addVertex(pos.x - half.x, pos.y - half.y, 0.f, 0.f, c);
        addVertex(pos.x + half.x, pos.y - half.y, size.x, 0.f, c);
        addVertex(pos.x + half.x, pos.y + half.y, size.x, size.y, c);
        addVertex(pos.x - half.x, pos.y + half.y, 0.f, size.y, c);
    }

    if (diff != mParticles.size())
    {
        std::cout << mParticles.size() << std::endl;
        diff = mParticles.size();
    }
}

//need to be fixed
void ParticleSystem::addVertex(float worldX, float worldY, float textX, float textY, sf::Color color) const
{
    sf::Vertex tempVertex;
    tempVertex.color = color;
    tempVertex.position = {worldX, worldY};
    tempVertex.texCoords = {textX, textY};
    mVertexArray.append(tempVertex);
}

void ParticleSystem::update(sf::Time dt)
{
    //remove all expired particles
    while (!mParticles.empty() && mParticles.front().lifetime <= sf::Time::Zero)
        mParticles.pop_front();

    //reduce time elapsed from particles' lifetime, and update pathindex
    int indexMax = mPath.size();
    for (auto &particle : mParticles)
    {
        particle.lifetime -= dt;
        particle.pathindex = (particle.pathindex + 1) % indexMax;
    }

    mNeedsUpdate = true;

    for (auto &affector : mAffector)
        affector(mParticles);
}

void ParticleSystem::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (mNeedsUpdate)
    {
        computeVertices();
        mNeedsUpdate = false;
    }

    states.texture = &mTexture;
    target.draw(mVertexArray, states);
}