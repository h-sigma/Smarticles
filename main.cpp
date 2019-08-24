#include <iostream>
#include <cmath>
#include "ParticleSystem.hpp"
#include "Emitter.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

int main()
{
    sf::Texture texture;
    texture.loadFromFile("assets/ParticleLong.png");
    texture.setSmooth(true);

    {
        sf::RenderTexture txtr;
        txtr.create(texture.getSize().x / 2, texture.getSize().y / 2);
        txtr.clear();
        sf::Sprite sprite(texture);
        sprite.scale(0.2f, 0.2f);
        txtr.draw(sprite);
        txtr.display();
        texture = txtr.getTexture();
    }

    sf::RenderWindow window(sf::VideoMode(800, 800), "ParticleDemo", sf::Style::Default);

    using namespace Attr;
    using PGreen = Radius<BaseParticle>;

    PGreen defaultGreen;
    defaultGreen.lifetime = sf::seconds(10);
    ParticleSystem<PGreen> sys(texture, sf::Color::Green, defaultGreen);

    std::cout << "\nsf::Time size " << sizeof(sf::Time) << '\n'
              << "sf::Color size " << sizeof(sf::Color) << '\n'
              << "sf::Vector2f size " << sizeof(sf::Vector2f) << '\n'
              << "Particle size " << sizeof(PGreen) << '\n'
              << "System size" << sizeof(ParticleSystem<PGreen>) << '\n';

    // sys.addAffector([](std::deque<Particle>&){
    // });
    auto affector = [center = window.getView().getCenter()](std::deque<PGreen> &particleList) {
        static int rotation = 0;
        rotation ++;
        const int maxRotation = 120; 
        float speed = particleList.size();
        speed /= 500;
        if(speed == 0)
            speed++;
        for (auto &particle : particleList)
        {
            sf::Vector2f len = particle.position - center;

            float angle = 0.01f * speed;
            len = rotate(len, angle);

            float variance = particle.radius.x;

            if(rotation > 30 && rotation < 90)
            variance = -variance;
            particle.position = len + center;
            particle.position += particle.position * variance;
        }
        rotation %= maxRotation;
    };

    int clusterNumber = 0;
    sf::Clock beatClock;
    sf::Time beat = sf::Time::Zero;
    const sf::Time beatDuration ( sf::seconds(.4f));
    auto finalizer = [&clusterNumber, &beatClock, &beat, &beatDuration](sf::VertexArray& varray){
        int len = varray.getVertexCount();
        const int gamma = 200 * 4;

        clusterNumber %= ((len+gamma)/gamma);
        sf::Time timeElapsed = beatClock.restart();
        if( beat > sf::Time::Zero)
        {
            beat -= timeElapsed;
            for(int i = 0 ; i < gamma && i+4 < len ; i+=4)
            {
                int k = i + clusterNumber * gamma;
                sf::Color c = sf::Color::White;

                float ratio = beat.asSeconds() / beatDuration.asSeconds();
                if(ratio > 0.9f)
                    ratio = 1.0 - ratio;
                else if(ratio > 0.1f)
                    ratio = 1.0;
                c.a = static_cast<uint8_t>(255 * std::max(0.0f, ratio));

                varray[k].color = c;
                varray[k+1].color = c;
                varray[k+2].color = c;
                varray[k+3].color = c;        
            }
        }
    };

    sys.addAffector(affector);
    sys.addFinalizer(finalizer);

    Emitter<PGreen> emit(defaultGreen);
    emit.setEmissionRate(200);
    emit.setPosition(window.getView().getCenter() + sf::Vector2f{50.f, 0.f});
    emit.setParticleSystem(&sys);

    auto emittermover = [center = window.getView().getCenter()](PGreen& particle, Emitter<PGreen>* emit){
        const int BAND_RADIUS = 100;
        sf::Vector2f len = emit->getPosition() - center;
        sf::Vector2f unit = (len)/(std::sqrt(len.x * len.x + len.y * len.y));
        particle.position = emit->getPosition() + unit * getFloat(0.f, BAND_RADIUS);
        particle.radius = {getFloat(-0.0416f, 0.0416f), 0.f };
        emit->setPosition(rotate(len, 1.f) + center);
    };

    emit.setParticleModifier(emittermover);

    bool running = true;

    sf::Clock clock;
    sf::Time timePerFrame = sf::seconds(1.f / 60);
    sf::Time dt = sf::Time::Zero;
    while (running)
    {
        //logic handling
        sf::Event event;
        bool spaceEvent;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                running = false;
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    running = false;
                else if (event.key.code == sf::Keyboard::Return)
                {
                    // for (int i = 0; i < 100; i++)
                    // {
                    //     PGreen particle = defaultGreen;
                    //     particle.position = {window.getDefaultView().getCenter() + sf::Vector2f{getFloat(200.f, 400.f), 0.f}};
                    //     sys.addParticle(particle);
                    // }
                }
                else if (event.key.code == sf::Keyboard::Space)
                {
                    spaceEvent = true;
                }
                else
                {
                    spaceEvent = false;
                }
                break;
            }
        }

        if(spaceEvent)
        {
                spaceEvent = false;
                beatClock.restart();
                beat = beatDuration;
                clusterNumber++;
        }

        //other logic
        dt += clock.restart();
        if (dt >= timePerFrame)
        {
            dt -= timePerFrame;
            emit.setEmissionRate((emit.getEmissionRate()+1.f));
            emit.update(dt);
            sys.update(timePerFrame);
        }

        window.clear();
        window.draw(sys);
        window.display();
    }
}

/*
void addParticle(sf::Vector2f position, Args const&... attr);
void addParticle(sf::Vector2f position, Args&&... attr);
those 2 will probably do then
one overload for lvalues and another for rvalues
same as push_back
you can also just do a single function taking by value 
void addParticle(sf::Vector2f position, Args... attr);
 and use the "sink idiom" if you're storing the attr somewhere
 */