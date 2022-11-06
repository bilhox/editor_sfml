#include "header/utils.hpp"

template<typename T , typename V>
bool collide_pr(sf::Rect<T> rect , sf::Vector2<V> pos){
    return (pos.x <= rect.left+rect.width) 
        && (pos.x >= rect.left)
        && (pos.y <= rect.top+rect.height)
        && (pos.y >= rect.top);
}