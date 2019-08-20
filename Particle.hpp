#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>

struct Particle{
    enum Type{
        Fire,
        ParticleCount
    };
    sf::Time lifetime = sf::seconds(1.f);      //8bytes
    sf::Vector2f position = {0.f,0.f};  //8bytes
    sf::Vector2f radius = {0.f,0.f};    //8bytes
    sf::Color color = sf::Color::Black;        //4bytes
    int pathindex = 0;
};

template <typename T = void> std::vector<Particle> initializeParticleData()
{
    std::vector<Particle> data(Particle::ParticleCount);

    data[Particle::Type::Fire].color = sf::Color::Red;
    data[Particle::Type::Fire].lifetime = sf::seconds(10.f);

    return data;
}

#endif