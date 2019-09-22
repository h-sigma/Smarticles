#ifndef EMITTER_HPP
#define EMITTER_HPP

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>

#include "Particle.hpp"
#include "ParticleSystem.hpp"

#include <functional>
#include <vector>


template <typename ParticleType = BaseParticle , typename InheritFrom = sf::Transformable>
class Emitter : public InheritFrom
{
public:
    explicit Emitter(ParticleType defaultParticle);
public:
    void update(sf::Time dt);
    void setDefaultParticle(ParticleType defaultParticle);
    void setEmissionRate(float rate);
    float getEmissionRate() const;
    void addParticleModifier(std::function<void(ParticleType&, Emitter<ParticleType>*)> modifier);
    void setParticleSystem(ParticleSystem<ParticleType>* system);
private:
    void emitParticles(sf::Time dt);
private:
    float mParticlesPerSecond = 300.f;
    ParticleSystem<ParticleType>* mParticleSystem;
    std::vector<std::function<void(ParticleType&, Emitter<ParticleType>*)>> mParticleModifiers;
    sf::Time mAccumulatedTime;
    ParticleType mDefaultParticle;
};

template<typename ParticleType, typename InheritFrom>
Emitter<ParticleType, InheritFrom>::Emitter(ParticleType defaultParticle)
: mParticleSystem(nullptr)
, mParticleModifier(nullptr)
, mAccumulatedTime(sf::Time::Zero)
, mDefaultParticle(defaultParticle)
{

}


template<typename ParticleType, typename InheritFrom>
void Emitter<ParticleType, InheritFrom>::setDefaultParticle(ParticleType defaultParticle)
{
    mDefaultParticle = defaultParticle;
}

template<typename ParticleType, typename InheritFrom>
void Emitter<ParticleType, InheritFrom>::emitParticles(sf::Time dt)
{
    sf::Time timeInterval = sf::seconds(1.f)/mParticlesPerSecond;

    mAccumulatedTime += dt;
    
    ParticleType particle(mDefaultParticle);
    while(mAccumulatedTime > timeInterval)
    {
        mAccumulatedTime -= timeInterval;
        for(auto& modifier : mParticleModifiers)
            modifier(particle, this);
        
        mParticleSystem->addParticle(particle);
    }
}

template<typename ParticleType, typename InheritFrom>
void Emitter<ParticleType, InheritFrom>::update(sf::Time dt)
{
    if(mParticleSystem)
        emitParticles(dt);
}

template<typename ParticleType, typename InheritFrom>
void Emitter<ParticleType, InheritFrom>::setEmissionRate(float rate)
{
    mParticlesPerSecond = rate;
}

template<typename ParticleType, typename InheritFrom>
float Emitter<ParticleType, InheritFrom>::getEmissionRate() const
{
    return mParticlesPerSecond;
}

template<typename ParticleType, typename InheritFrom>
void Emitter<ParticleType, InheritFrom>::addParticleModifier(std::function<void(ParticleType&, Emitter<ParticleType>*)> modifier)
{
    mParticleModifier.push_back(modifier);
}

template<typename ParticleType, typename InheritFrom>
void Emitter<ParticleType, InheritFrom>::setParticleSystem(ParticleSystem<ParticleType>* system)
{
    mParticleSystem = system;
}


#endif
