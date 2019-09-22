#ifndef PARTICLESYS_HPP
#define PARTICLESYS_HPP

#include "Particle.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <vector>
#include <deque>
#include <functional>

int getInt(int a, int b);

//TEMPLATE DECLARATION
template <typename ParticleType = BaseParticle<>>
class ParticleSystem : public sf::Drawable
{
public:
    ParticleSystem(sf::Texture &texture, sf::Color defaultColor, ParticleType particle);
    void addParticle();
    void addParticle(ParticleType const& particle);
    void addParticle(ParticleType&& particle);
    void addAffector(std::function<void(std::deque<ParticleType> &)> affector);
    void setLifetime(sf::Time lifetime);
    void addFinalizer(std::function<void(sf::VertexArray &)> finalizer);
    void update(sf::Time dt);
public:
    ParticleType getDefaultParticle() const;
    unsigned int getParticleCount() const;
private:
    void computeVertices() const;
    void addVertex(float worldX, float worldY, float textX, float textY, sf::Color color) const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

protected:
    std::deque<ParticleType> mParticles;
    mutable sf::VertexArray mVertexArray;

    std::vector<std::function<void(std::deque<ParticleType> &)>> mAffector;
    std::vector<std::function<void(sf::VertexArray&)>> mFinalizer;
    
    mutable bool mNeedsUpdate = true;

    sf::Color mDefaultColor;
    ParticleType mDefaultParticle;
    sf::Texture &mTexture;
};

// TEMPLATE DEFINITIONS

template <typename ParticleType>
ParticleSystem<ParticleType>::ParticleSystem(sf::Texture &texture, sf::Color defaultColor, ParticleType particle)
    : mTexture(texture), mVertexArray(sf::Quads), mDefaultParticle(particle), mDefaultColor(defaultColor)
{

}

template <typename ParticleType>
void ParticleSystem<ParticleType>::addAffector(std::function<void(std::deque<ParticleType> &)> affector)
{
    mAffector.push_back(affector);
}

template <typename ParticleType>
void ParticleSystem<ParticleType>::addFinalizer(std::function<void(sf::VertexArray &)> finalizer)
 {
    mFinalizer.push_back(finalizer);
 }
template<typename ParticleType>
void ParticleSystem<ParticleType>::addParticle()
{
    mParticles.push_back(mDefaultParticle);
}

template <typename ParticleType> 
void ParticleSystem<ParticleType>::addParticle(ParticleType&& particle)
{
    mParticles.push_back(std::move(particle));
}

template <typename ParticleType>
void ParticleSystem<ParticleType>::addParticle(ParticleType const& particle)
{
    mParticles.push_back(particle);
}

template <typename ParticleType>
void ParticleSystem<ParticleType>::setLifetime(sf::Time lifetime)
{
    mDefaultParticle.lifetime = lifetime;
}

template <typename ParticleType>
ParticleType ParticleSystem<ParticleType>::getDefaultParticle() const
{
    return mDefaultParticle;
}

template <typename ParticleType>
unsigned int ParticleSystem<ParticleType>::getParticleCount() const
{
    return mParticles.size();
}

template <typename ParticleType>
void ParticleSystem<ParticleType>::computeVertices() const
{
    static int diff = 0;

    sf::Vector2f size(mTexture.getSize());
    sf::Vector2f half = size / 2.f;

    mVertexArray.clear();

    for (const auto &particle : mParticles)
    {
        sf::Vector2f pos = particle.position;
        sf::Color c;
        if constexpr(attr::has_color_v<ParticleType>)
        {
            c = particle.color;
        }
        else 
        {
            c = mDefaultColor;
            const float ratio = particle.lifetime.asSeconds() / mDefaultParticle.lifetime.asSeconds();
            c.a = static_cast<uint8_t>(255 * std::max(0.0f, ratio)); //can't forget to keep the alpha value positive
        }
        
        addVertex(pos.x - half.x, pos.y - half.y, 0.f, 0.f, c);
        addVertex(pos.x + half.x, pos.y - half.y, size.x, 0.f, c);
        addVertex(pos.x + half.x, pos.y + half.y, size.x, size.y, c);
        addVertex(pos.x - half.x, pos.y + half.y, 0.f, size.y, c);
    }

    if (diff != mParticles.size())
    {
        std::cout << mParticles.size() << " " << mVertexArray.getVertexCount() << std::endl;
        diff = mParticles.size();
    }
}

//need to be fixed
template <typename ParticleType>
void ParticleSystem<ParticleType>::addVertex(float worldX, float worldY, float textX, float textY, sf::Color color) const
{
    sf::Vertex tempVertex;
    tempVertex.color = color;
    tempVertex.position = {worldX, worldY};
    tempVertex.texCoords = {textX, textY};
    mVertexArray.append(tempVertex);
}

template <typename ParticleType>
void ParticleSystem<ParticleType>::update(sf::Time dt)
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

template <typename ParticleType>
void ParticleSystem<ParticleType>::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (mNeedsUpdate)
    {
        computeVertices();
        for(auto& finalizer : mFinalizer)
            finalizer(mVertexArray);
        mNeedsUpdate = false;
    }

    states.texture = &mTexture;
    target.draw(mVertexArray, states);
}

#endif
