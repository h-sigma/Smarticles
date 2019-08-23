#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <SFML/System/Vector2.hpp>
#include <iterator>
#include <type_traits>

int getInt(int a, int b);

float getFloat(float a, float b);

sf::Vector2f rotate(sf::Vector2f vector, float angle);

template<typename iter1, typename iter2, typename Function> 
void pairIterate(iter1 begin1, iter1 end1, iter2 begin2, iter2 end2, Function func)
{
    for(; begin1 != end1 && begin2 != end2 ; std::advance(begin1, 1) , std::advance(begin2,2))
        func(*begin1, *begin2);
}

template<typename Container, typename Iter = typename Container::iterator>
Iter interpolate(Container& container, Iter pos, int count)
{
    if(pos == container.begin())
        return container.end();
    auto left = *(pos-1);
    auto step = (*(pos) - left)/(count+1);
    Iter returnee = container.insert(pos, count, std::decay_t<decltype(*pos)>{});
    Iter giveValue = returnee;
    for(int i = 1; i <= count ; i++)
    {
        *(giveValue++) = left + (step * i);
    }
    return returnee;
}


// -> std::invoke_result_t< Function , typename std::iterator_traits<iter1>::value_type, typename 
// std::iterator_traits<iter2>::value_type>

#endif