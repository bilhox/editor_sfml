#include "header/component.hpp"
#include <iostream>

ui::Component::Component(){

    this->m_vertices.setPrimitiveType(sf::TriangleStrip);
    this->m_vertices.resize(4);

    this->set_color({255,255,255});

}

void ui::Component::update(float dt){

}
bool ui::Component::events(sf::Event& event , sf::Window& window){
    return false;
}

std::shared_ptr<ui::Component> ui::Component::operator[](std::size_t idx){
    if(idx < 0 || idx >= std::size(m_children))
        throw std::out_of_range("No child at index : "+std::to_string(idx));
    
    return m_children[idx];
}

sf::Cursor::Type ui::Component::getCursorState(sf::Vector2i mpos){
    return sf::Cursor::Arrow;
}

sf::FloatRect ui::Component::get_rect(){
    return this->rect;
}

void ui::Component::setPosition(const sf::Vector2f& position){
    Transformable::setPosition(position);
    this->rect.left = position.x;
    this->rect.top = position.y;
}

void ui::Component::setPosition(float x , float y){
    Transformable::setPosition(x , y);
    this->rect.left = x;
    this->rect.top = y;
}

void ui::Component::set_color(sf::Color color){

    this->color = color;

    for(int i = 0 ; i < 4 ; i++){
        sf::Vertex* vertex = &this->m_vertices[i];

        vertex->color = color;
    }
}

const sf::Vector2f ui::Component::getSize() const {
    return this->rect.getSize();
}

sf::Color ui::Component::get_color(){
    return this->color;
}

const sf::Vector2f ui::Component::getPosition(bool global) const {
    if(global){
        const sf::Vector2f localpos = Transformable::getPosition();
        const sf::Vector2f parentPos = ((this->parent)?this->parent->getPosition(true):sf::Vector2f(0,0));
        return parentPos+localpos;
    }
    
    return Transformable::getPosition();
}

void ui::Component::updatePointers(){
    if(std::size(m_children) != 0){
        for(auto & child : m_children){
            child->parent = this;
            child->updatePointers();
        }
    }
}

const sf::Vector2f ui::Component::getMinimumSize() const{
    return m_minSize;
}

void ui::Component::resize(sf::Vector2f size){
    this->rect.width = size.x;
    this->rect.height = size.y;

    for(int i=0;i<4;i++){
        sf::Vertex *vertex = &this->m_vertices[i];

        vertex->position = sf::Vector2f((i%2)*size.x , (i/2)*size.y);
        vertex->texCoords = sf::Vector2f((i%2)*size.x , (i/2)*size.y);
    }
}

void ui::Component::display(sf::RenderWindow& window){
    
    auto size = this->rect.getSize();
    if(size.x < m_minSize.x || size.y < m_minSize.y){
        return;
    }

    auto winsize{window.getSize()};

    sf::View viewrect {this->rect};

    viewrect.setViewport({
        (this->getPosition(true).x) / winsize.x,
        (this->getPosition(true).y) / winsize.y,
        this->rect.width / winsize.x,
        this->rect.height / winsize.y
    });

    sf::View winview {window.getView()};
    window.setView(viewrect);
    window.draw(*this);
    window.setView(winview);
}

void ui::Component::draw(sf::RenderTarget& target , sf::RenderStates& states) const {

    states.transform *= this->getTransform();
    states.texture = &this->m_texture;

    target.draw(this->m_vertices , states);
}

bool ui::Component::loadTexture(std::string path){
    if(!this->m_texture.loadFromFile(path)){
        return false;
    }
    return true;
}