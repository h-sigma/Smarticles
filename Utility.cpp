#include "Utility.hpp"
#include <cmath>




sf::Vector2f rotate(sf::Vector2f vector, float angle)
{
    sf::Vector2f rot{std::cos(angle), std::sin(angle)};
    return {vector.x * rot.x - vector.y * rot.y, vector.y * rot.x + vector.x * rot.y};
}

