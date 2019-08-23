#include <iostream>
#include <cmath>
#include "ParticleSystem.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

int main()
{
    std::vector<float> fl{1.f, 2.f};
    interpolate(fl, fl.begin() + 1, 9);
    for(auto f : fl)
        std::cout << f << " ";

    std::cout << "\nsf::Time size " << sizeof(sf::Time) << '\n'
              << "sf::Color size " << sizeof(sf::Color) << '\n'
              << "sf::Vector2f size " << sizeof(sf::Vector2f) << '\n'
              << "Particle size " << sizeof(Particle<>) << '\n'
              << "System size" << sizeof(ParticleSystem<>) << '\n';
    sf::Texture texture;
    texture.loadFromFile("assets/Particle.png");
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

    sf::RenderWindow window(sf::VideoMode(500, 500), "ParticleDemo", sf::Style::Default);

    ParticleSystem<> sys(texture, sf::seconds(30.f), sf::Color::Red);
    ParticleSystem<> sys2(texture, sf::seconds(30.f), sf::Color::Green);

    // sys.addAffector([](std::deque<Particle>&){
    // });
    auto affector = [center = window.getView().getCenter()](ParticleSystem<>::aliveList &particleList) {
        for (auto &particle : particleList)
        {
            sf::Vector2f len = particle.position - center;
            len = rotate(len, getFloat(0.001f, 0.05f));
            particle.position = len + center;
        }
    };

    sys.addAffector(affector);
    sys2.addAffector(affector);

    bool running = true;

    sf::Clock clock;
    sf::Time timePerFrame = sf::seconds(1.f / 60);
    sf::Time dt = sf::Time::Zero;
    while (running)
    {
        //logic handling
        sf::Event event;
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
                    for (int i = 0; i < 5; i++)
                    {
                        sys.addParticle(window.getDefaultView().getCenter() + sf::Vector2f{getFloat(0.f,100.f) , 0.f});
                        sys2.addParticle(window.getDefaultView().getCenter() - sf::Vector2f{getFloat(100.f,200.f) , 0.f});
                    }
                }
                break;
            }
        }
        //other logic
        dt += clock.restart();
        if (dt >= timePerFrame)
        {
            dt -= timePerFrame;
            sys.update(timePerFrame);
            sys2.update(timePerFrame);
        }

        window.clear();
        window.draw(sys);
        window.draw(sys2);
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