#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>

template<typename T , typename V>
bool collide_pr(sf::Rect<T> rect , sf::Vector2<V> pos);

#include "../utils.tpp"

#endif // UTILS_HPP