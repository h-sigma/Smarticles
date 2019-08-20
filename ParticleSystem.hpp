#ifndef PARTICLESYS_HPP
#define PARTICLESYS_HPP

#include "Particle.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <vector>
#include <deque>
#include <functional>

int getInt(int a, int b);

namespace sf
{
class RenderStates;
class RenderTexture;
} // namespace sf

class ParticleSystem : public sf::Drawable
{
public:
    ParticleSystem(Particle::Type type, sf::Texture &texture);
    void addPath(sf::Vector2f point);
    void addPath(float x, float y);
    void addParticle(sf::Vector2f position);
    // template<typename... T> void addParticle(T... args)
    // {
    //     mParticles.emplace_back(std::forward<T>(args));
    // }
    void addAffector(std::function<void(std::deque<Particle> &)> affector);
    void update(sf::Time dt);

private:
    void computeVertices() const;
    void addVertex(float worldX, float worldY, float textX, float textY, sf::Color color) const;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

protected:
    Particle::Type mType;
    sf::Texture &mTexture;

    std::deque<Particle> mParticles;
    mutable sf::VertexArray mVertexArray;
    std::vector<sf::Vector2f> mPath;

    std::vector<std::function<void(std::deque<Particle> &)>> mAffector;
    mutable bool mNeedsUpdate = true;
};

#endif