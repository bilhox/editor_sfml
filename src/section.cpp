#include "header/section.hpp"
#include "header/utils.hpp"
#include <numeric>

ui::Section::Section(sf::Vector2f size , ui::Direction flexDirection){
    this->resize(size);
    this->m_flexDirection = flexDirection;
}

void ui::Section::updateChildrenSize(){
    auto size = this->getSize();
    if(std::size(m_children) != 0){
        
        float offset = 0;
        int i = 0;
        if(std::size(m_children) != 0){
            for(auto & child : m_children){
            if(m_flexDirection == ui::Direction::Horizontal){
                child->resize({size.x*m_weights[i],size.y});
                child->setPosition({offset , 0});
                offset += size.x*m_weights[i];
            }
            else {
                child->resize({size.x,size.y*m_weights[i]});
                child->setPosition({0 , offset});
                offset += size.y*m_weights[i];
            }
            i++;
            }
        }

    }
}

void ui::Section::resize(sf::Vector2f size){
    ui::Component::resize(size);
    updateChildrenSize();
}

void ui::Section::setWeights(std::vector<float> weights){
    if((std::size(weights) != std::size(m_children)) && std::accumulate(weights.begin() , weights.end() , 0.f) > 1)
        return;

    m_weights = weights;
    updateChildrenSize();
}

sf::Cursor::Type ui::Section::getCursorState(sf::Vector2i mpos){

    sf::Vector2f fmpos {mpos};

    if(std::size(m_children) != 0){
        for(auto & child : m_children){
        if(collide_pr(child->get_rect() , fmpos-this->getPosition(true))){
            return child->getCursorState(mpos);
        }
        }
    }

    return sf::Cursor::Arrow;
}


void ui::Section::update(float dt){
    for(auto & child : m_children){
        child->update(dt);
    }
}

void ui::Section::setFlexDirection(ui::Direction direction){
    m_flexDirection = direction;
    updateChildrenSize();
}

bool ui::Section::events(sf::Event & event , sf::Window & window){
    for(auto & child : m_children){
        if(child->events(event , window))
            return true;
    }
    return false;
}

void ui::Section::addChild(std::shared_ptr<ui::Component> component_ptr){

    m_children.emplace_back(std::move(component_ptr));
    // std::cout << "Father : " << &*this << " ; Child : " << &component_ptr.get() << std::endl;
    m_weights.push_back(1);
    this->updatePointers();

    if(std::size(m_weights) != 0){
        for(auto & weight : m_weights){
            weight = 1/(float)std::size(m_children);
        }
    }

    sf::Vector2f minSize {0,0};
    for(auto & child : m_children){
        auto childMinSize = child->getMinimumSize();
        if(childMinSize.x > minSize.x)
            minSize.x = childMinSize.x;
        
        if(childMinSize.y > minSize.y)
            minSize.y = childMinSize.y;
    }

    m_minSize = minSize;
    updateChildrenSize();
}

void ui::Section::display(sf::RenderWindow & window){
    ui::Component::display(window);
    for(auto & child : m_children){
        child->display(window);
    }
}

void ui::Section::draw(sf::RenderTarget& target , sf::RenderStates states) const{
    states.transform *= getTransform();
    states.texture = &m_texture;
    
    target.draw(m_vertices , states);
}