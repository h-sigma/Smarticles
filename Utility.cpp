#include "Utility.hpp"
#include <cmath>

#include <random>

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

float getFloat(float a, float b)
{
    std::uniform_real_distribution<> dist(a, b);
    return dist(engine);
}

sf::Vector2f rotate(sf::Vector2f vector, float angle)
{
    sf::Vector2f rot{std::cos(angle), std::sin(angle)};
    return {vector.x * rot.x - vector.y * rot.y, vector.y * rot.x + vector.x * rot.y};
}

