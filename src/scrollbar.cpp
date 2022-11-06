#include "header/scrollbar.hpp"
#include <iostream>
#include "header/utils.hpp"

ui::ScrollBar::ScrollBar(float length , ui::Direction direction){
    
    this->direction = direction;

    this->set_color({92, 160, 255});

    
    if(this->direction == ui::Direction::Vertical){
        this->resize({20 , length});
        this->bar = sf::RectangleShape{{20 , 0}};
    }
    else {
        this->resize({length , 20});
        this->bar = sf::RectangleShape{{0 , 20}};
    }
    this->bar.setPosition({0,0});
    this->bar.setFillColor(sf::Color::Magenta);

}

void ui::ScrollBar::resize(sf::Vector2f size){
    ui::Component::resize(size);
    if (this->direction == ui::Direction::Vertical){
        this->length = size.y;
    }
    else {
        this->length = size.x;
    }
}

void ui::ScrollBar::update(float dt){

}

void ui::ScrollBar::setBarColor(sf::Color color){
    this->bar.setFillColor(color);
}

void ui::ScrollBar::setBarLength(float bar_length){
    if(bar_length < 0)
        return;

    if(this->direction == ui::Direction::Vertical){
        float temp_val = (this->movability_distance != 0)?this->bar.getPosition().y / this->movability_distance:0;
        this->bar.setSize({20 , bar_length});
        this->bar_visible = (bar_length <= this->length);
        if(this->bar_visible){
            this->movability_distance = this->rect.height - bar_length;
            this->bar.setPosition({0,this->movability_distance*temp_val});
        }
        else {
            this->movability_distance = 0.f;
            this->bar.setPosition({0,0});
        }
    }
    else {
        float temp_val = (this->movability_distance != 0)?this->bar.getPosition().x / this->movability_distance:0;
        this->bar.setSize({bar_length , 20});
        this->bar_visible = (bar_length <= this->length);
        if(this->bar_visible){
            this->movability_distance = this->rect.width - bar_length;
            this->bar.setPosition({this->movability_distance*temp_val,0});
        }
        else {
            this->movability_distance = 0.f;
            this->bar.setPosition({0,0});
        }
    }


}

float ui::ScrollBar::getMovabilityDistance(){
    return this->movability_distance;
}

float ui::ScrollBar::getRemainingMovabilityDistance(){
    return (this->bar_visible)?((this->direction == ui::Direction::Vertical)?this->bar.getPosition().y:this->bar.getPosition().x):0;
}

bool ui::ScrollBar::isBarVisible(){
    return this->bar_visible;
}

bool ui::ScrollBar::events(sf::Event & event , sf::Window & window){


    if(!this->bar_visible)
        return false;

    sf::Vector2f mpos {sf::Mouse::getPosition(window)};

    if(this->parent){
        mpos -= this->parent->getPosition(true);
    }

    switch(event.type){
        
        case sf::Event::MouseButtonPressed:
            mpos -= this->rect.getPosition();
            if(event.mouseButton.button == sf::Mouse::Left){
                if(collide_pr(this->bar.getGlobalBounds() , mpos)){
                    this->bar_moved = true;
                    
                    if(this->direction == ui::Direction::Vertical){
                        this->mouse_diff = mpos.y - this->bar.getPosition().y;
                    }
                    else {
                        this->mouse_diff = mpos.x - this->bar.getPosition().x;
                    }
                    return true;
                }
            }
            break;

        case sf::Event::MouseMoved:
            if(this->bar_moved && collide_pr(this->rect , mpos)){
                if(this->direction == ui::Direction::Vertical){
                    float offset = mpos.y-this->rect.top;
                    float ypos = offset-this->mouse_diff;
                    
                    this->bar.setPosition({0 , ypos});
                    if(this->bar.getPosition().y < 0){
                        this->bar.setPosition({0 , 0});
                    } else if (this->bar.getPosition().y+this->bar.getSize().y > this->length){
                        this->bar.setPosition({0 , this->length - this->bar.getSize().y});
                    }else {
                        this->mouse_diff = offset - this->bar.getPosition().y;
                    }
                } else {
                    float offset = mpos.x-this->rect.left;
                    float xpos = offset-this->mouse_diff;
                    
                    this->bar.setPosition({xpos , 0});
                    if(this->bar.getPosition().x < 0){
                        this->bar.setPosition({0 , 0});
                    } else if (this->bar.getPosition().x+this->bar.getSize().x > this->length){
                        this->bar.setPosition({this->length - this->bar.getSize().x , 0});
                    }else {
                        this->mouse_diff = offset - this->bar.getPosition().x;
                    }
                }
                return true;
            }
            break;
        
        case sf::Event::MouseButtonReleased:
            if(this->bar_moved){
                this->bar_moved = false;
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

sf::Cursor::Type ui::ScrollBar::getCursorState(sf::Vector2i mpos){
    sf::Vector2f fmpos {mpos};

    if(this->parent){
        fmpos -= this->parent->getPosition(true);
    }

    if(this->bar_visible){
        fmpos -= this->getPosition();

        if(collide_pr(this->bar.getGlobalBounds() , fmpos)){
            return sf::Cursor::Hand;
        }
    }
    
    return sf::Cursor::Arrow;
}

void ui::ScrollBar::synchronizeBar(float distance){
    if(this->bar_visible){
        if(distance <= this->movability_distance && distance >= 0){
            if(this->direction == ui::Direction::Vertical){
                this->bar.setPosition({0 , distance});
            }
            else {
                this->bar.setPosition({distance , 0});
            }
        }
        else {
            if(this->direction == ui::Direction::Vertical){
                this->bar.setPosition({0 , this->movability_distance});
            }
            else {
                this->bar.setPosition({this->movability_distance , 0});
            }
        }
    }
}


void ui::ScrollBar::draw(sf::RenderTarget & target , sf::RenderStates states) const {

    states.transform *= this->getTransform();
    states.texture = NULL;

    target.draw(this->m_vertices , states);
    if(this->bar_visible){
        target.draw(this->bar , states);
    }
}