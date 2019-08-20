#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>
#include <cstdint>

// enum class Tags : uint32_t {
//     position    = 1u << 0u,
//     radius      = 1u << 1u,
//     color       = 1u << 2u,
//     lifetime    = 1u << 3u,
//     pathindex   = 1u << 4u,
// };

// constexpr Tags operator|(Tags a, Tags b)
// {
//     return static_cast<Tags>( static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
// }

// constexpr bool operatorbool(Tags a)
// {
//     return static_cast<bool>(static_cast<uint32_t>(a));
// }

// struct Particle{
//     static constexpr Tags tag = Tags::position | Tags::radius | Tags::lifetime | Tags::color | Tags::pathindex;
//     sf::Time lifetime = sf::seconds(1.f);      //8bytes
//     sf::Vector2f position = {0.f,0.f};  //8bytes
//     sf::Vector2f radius = {0.f,0.f};    //8bytes
//     sf::Color color = sf::Color::Black;        //4bytes
//     int pathindex = 0;  //4 bytes
// };

template<typename... T>
struct Particle
{
    //sf::Color color;                
    sf::Vector2f position;
    sf::Time lifetime;           //   1
    std::tuple<T...> attr;       //   2...n
};


#endif