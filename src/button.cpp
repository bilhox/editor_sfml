#include "header/button.hpp"
#include "header/utils.hpp"

void ui::Button::setText(std::string txt){
    this->text.setPosition({0,0});
    this->text.setString(txt);
    this->text.setOrigin(sf::Vector2f(this->text.getLocalBounds().getPosition() + this->text.getGlobalBounds().getSize()/2.f));
    this->text.setPosition(this->rect.getSize()/2.f);
}

void ui::Button::setTextCharacterSize(int size){
    this->text.setPosition({0,0});
    this->text.setCharacterSize(size);
    this->text.setOrigin(sf::Vector2f(this->text.getLocalBounds().getPosition() + this->text.getGlobalBounds().getSize()/2.f));
    this->text.setPosition(this->rect.getSize()/2.f);
}

ui::Button::Button(sf::Vector2f size , std::string text){

    m_minSize = sf::Vector2f(10,10);

    this->text.setString(text);
    this->text.setCharacterSize(20);
    this->font.loadFromFile("../assets/font/static/RobotoMono-Regular.ttf");
    this->text.setFont(this->font);
    this->text.setLineSpacing(-1);
    this->text.setOrigin(sf::Vector2f(this->text.getLocalBounds().getPosition() + this->text.getGlobalBounds().getSize()/2.f));

    this->resize(size);
    this->set_colors({sf::Color::White , sf::Color::White , sf::Color::White});
    this->set_color(this->colors[0]);
}

sf::Vector2f ui::Button::get_size(){
    return this->rect.getSize();
}

void ui::Button::resize(sf::Vector2f size){

    ui::Component::resize(size);
    this->text.setPosition(this->rect.getSize()/2.f);
}

void ui::Button::set_colors(std::array<sf::Color , 3> colors){
    this->colors = colors;
}

sf::Cursor::Type ui::Button::getCursorState(sf::Vector2i mpos){
    return sf::Cursor::Hand;
}

bool ui::Button::events(sf::Event &event , sf::Window &window){

    sf::Vector2f mpos {sf::Mouse::getPosition(window)};
    if(this->parent)
        mpos -= this->parent->getPosition(true);
    
    
    switch(event.type){
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left){
                if(collide_pr(this->rect , mpos)){

                    this->state = State::CLICKED;
                    return true;
                }       
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left){
                if(collide_pr(this->rect , mpos) && this->state == State::CLICKED){
                    try{
                        this->target();
                    } catch (std::exception const & error){
                        std::cout << "Error : " << error.what() << std::endl;
                        std::cout << "Object : " << &*this << std::endl;
                    }
                    this->state = State::HOVER;
                    return true;
                } else{
                    this->state = State::NORMAL;
                }
                
            }
            break;
        case sf::Event::MouseMoved:
            if (this->state != State::CLICKED){
                if(collide_pr(this->rect, mpos)){
                    this->state = State::HOVER;
                }else{
                    this->state = State::NORMAL;
                }
            } else if (this->state == State::CLICKED){
                return true;
            }
        
            break;
        
        default:
            break;
    }
    return false;
}

void ui::Button::update(float dt){
    switch(this->state){
        case State::NORMAL:
            this->set_color(this->colors[0]);
            break;
        case State::HOVER:
            this->set_color(this->colors[1]);
            break;
        case State::CLICKED:
            this->set_color(this->colors[2]);
            break;
    }
}

void ui::Button::draw(sf::RenderTarget &target , sf::RenderStates states) const{
    states.transform *= this->getTransform();
    states.texture = &this->m_texture;

    target.draw(this->m_vertices , states);
    target.draw(this->text , states);
}