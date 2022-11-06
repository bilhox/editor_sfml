#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>
#include "component.hpp"
#include <string>
#include <iostream>
#include <array>
#include <functional>

namespace ui{

    class Button : public ui::Component
    {
    public:

        std::function<void()> target;

        Button(sf::Vector2f size , std::string text);
        void update(float dt);
        bool events(sf::Event &event , sf::Window& window);
        void resize(sf::Vector2f size);
        void set_colors(std::array<sf::Color , 3> colors);
        sf::Cursor::Type getCursorState(sf::Vector2i mpos);

        void setText(std::string txt);
        void setTextCharacterSize(int size);
        sf::Vector2f get_size();

    private:

        enum State{
            HOVER,
            CLICKED,
            NORMAL
        };

        std::array<sf::Color,3> colors;
        sf::Texture m_texture;
        State state;
        sf::Font font;
        sf::Text text;
        
        virtual void draw(sf::RenderTarget &target , sf::RenderStates states) const;

    };
}

#endif