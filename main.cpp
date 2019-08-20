#include <iostream>
#include "Particle.hpp"
#include "ParticleSystem.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

int main()
{
    std::cout << "sf::Time size " << sizeof(sf::Time) << '\n'
              << "sf::Color size " << sizeof(sf::Color) << '\n'
              << "sf::Vector2f size " << sizeof(sf::Vector2f) << '\n'
              << "Particle size " << sizeof(Particle<>) << '\n'
              << "System size" << sizeof(ParticleSystem<>) << '\n';
    sf::Texture texture;
    texture.loadFromFile("assets/Particle.png");
    texture.setSmooth(true);
    {
        sf::RenderTexture txtr;
        txtr.create(texture.getSize().x/2, texture.getSize().y/2);
        txtr.clear();
        sf::Sprite sprite(texture);
        sprite.scale(0.5f,0.5f);
        txtr.draw(sprite);
        txtr.display();
        texture = txtr.getTexture();
    }

    ParticleSystem<> sys(texture, sf::seconds(5.f), sf::Color::Red);

    // sys.addAffector([](std::deque<Particle>&){
    // });
    sys.addAffector([](ParticleSystem<>::aliveList& particleList)
    {
        for(auto& particle : particleList)
        {
            particle.position.x++;
            particle.position.y++;
        }
    });

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "ParticleDemo", sf::Style::Default);

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
                    for (int i = 0; i < 100; i++)
                        sys.addParticle({});
                }
                break;
            }
        }
        //other logic
        dt += clock.restart();
        if (dt >= timePerFrame)
        {
            dt -= timePerFrame;
            sys.addParticle({});
            sys.update(timePerFrame);
        }

        window.clear();
        window.draw(sys);
        window.display();
    }
}