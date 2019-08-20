#ifndef PARTICLESYS_HPP
#define PARTICLESYS_HPP

#include "Particle.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>


#include <random>
#include <vector>
#include <deque>
#include <functional>

int getInt(int a, int b);

//TEMPLATE DECLARATION
template <typename... Args>
class ParticleSystem : public sf::Drawable
{
public:
    using aliveList = std::deque<Particle<Args...>>;
public:
    ParticleSystem(sf::Texture &texture, sf::Time defaultLifetime, sf::Color defaultColor, Args... defaultParticleAttr);
    void addParticle(sf::Vector2f position, Args... attr);
    void addAffector(std::function<void(aliveList &)> affector);
    void setLifetime(sf::Time lifetime);
    void addShader(std::function<void(sf::VertexArray &)> shader);
    void update(sf::Time dt);
private:
    void computeVertices() const;
    void addVertex(float worldX, float worldY, float textX, float textY, sf::Color color) const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

protected:
    sf::Texture &mTexture;

    aliveList mParticles;
    mutable sf::VertexArray mVertexArray;

    std::vector<std::function<void(aliveList &)>> mAffector;
    std::vector<std::function<void(sf::VertexArray&)>> mShaders;
    
    mutable bool mNeedsUpdate = true;

    sf::Color mDefaultColor;
    Particle<Args...> mDefaultParticle;
};

// TEMPLATE DEFINITIONS

namespace
{
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

template <typename... Args>
ParticleSystem<Args...>::ParticleSystem(sf::Texture &texture, sf::Time defaultLifetime, sf::Color defaultColor, Args... defaultParticleArgs)
    : mTexture(texture), mVertexArray(sf::Quads), mDefaultParticle({}, defaultLifetime, std::forward<Args...>(defaultParticleArgs...))
{
}

template <typename... Args>
void ParticleSystem<Args...>::addAffector(std::function<void(aliveList &)> affector)
{
    mAffector.push_back(affector);
}

template <typename... Args>
 void ParticleSystem<Args...>::addShader(std::function<void(sf::VertexArray &)> shader)
 {
    mShaders.push_back(shader);
 }

template <typename... Args>
void ParticleSystem<Args...>::addParticle(sf::Vector2f position, Args... attr)
{
    Particle<Args...> particle;
    particle.position = position;
    particle.lifetime = mDefaultParticle.lifetime;
    mParticles.emplace_back(position, mDefaultParticle.lifetime, attr...);
}

template <typename... Args>
void ParticleSystem<Args...>::setLifetime(sf::Time lifetime)
{
    mDefaultParticle.lifetime = lifetime;
}

template <typename... Args>
void ParticleSystem<Args...>::computeVertices() const
{
    static int diff = 0;

    sf::Vector2f size(mTexture.getSize());
    sf::Vector2f half = size / 2.f;

    mVertexArray.clear();

    for (const auto &particle : mParticles)
    {
        sf::Vector2f pos = particle.position;
        sf::Color c = mDefaultColor;
        const float ratio = particle.lifetime.asSeconds() / mDefaultParticle.lifetime.asSeconds();
        c.a = static_cast<uint8_t>(255 * std::max(0.0f, ratio)); //can't forget to keep the alpha value positive

        addVertex(pos.x - half.x, pos.y - half.y, 0.f, 0.f, c);
        addVertex(pos.x + half.x, pos.y - half.y, size.x, 0.f, c);
        addVertex(pos.x + half.x, pos.y + half.y, size.x, size.y, c);
        addVertex(pos.x - half.x, pos.y + half.y, 0.f, size.y, c);
    }

    // if (diff != mParticles.size())
    // {
    //     std::cout << mParticles.size() << std::endl;
    //     diff = mParticles.size();
    // }
}

//need to be fixed
template <typename... Args>
void ParticleSystem<Args...>::addVertex(float worldX, float worldY, float textX, float textY, sf::Color color) const
{
    sf::Vertex tempVertex;
    tempVertex.color = color;
    tempVertex.position = {worldX, worldY};
    tempVertex.texCoords = {textX, textY};
    mVertexArray.append(tempVertex);
}

template <typename... Args>
void ParticleSystem<Args...>::update(sf::Time dt)
{
    //remove all expired particles
    while (!mParticles.empty() && mParticles.front().lifetime <= sf::Time::Zero)
        mParticles.pop_front();

    //reduce time elapsed from particles' lifetime
    for (auto &particle : mParticles)
    {
        particle.lifetime -= dt;
    }

    mNeedsUpdate = true;

    for (auto &affector : mAffector)
        affector(mParticles);
}

template <typename... Args>
void ParticleSystem<Args...>::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (mNeedsUpdate)
    {
        computeVertices();
        for(auto& shader : mShaders)
            shader(mVertexArray);
        mNeedsUpdate = false;
    }

    states.texture = &mTexture;
    target.draw(mVertexArray, states);
}

#endif