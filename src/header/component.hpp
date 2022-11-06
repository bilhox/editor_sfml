#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

namespace ui {
    
    class Component :  public sf::Drawable , public sf::Transformable {
        public:

            ui::Component* parent = nullptr;

            Component();
            virtual void update(float dt);
            virtual bool events(sf::Event &event , sf::Window &window);
            void setPosition(const sf::Vector2f& position);
            void setPosition(float x , float y);
            const sf::Vector2f getPosition(bool global = false) const;
            const sf::Vector2f getSize() const;
            virtual void resize(sf::Vector2f size);
            void set_color(sf::Color color);
            sf::Color get_color();
            sf::FloatRect get_rect();
            bool loadTexture(std::string path);
            virtual sf::Cursor::Type getCursorState(sf::Vector2i mpos);
            virtual void display(sf::RenderWindow& window);
            void updatePointers();
            const sf::Vector2f getMinimumSize() const;

        protected:

            sf::FloatRect rect;
            sf::Vector2f m_minSize;
            sf::VertexArray m_vertices;
            sf::Color color;
            sf::Texture m_texture;
            std::vector<std::shared_ptr<ui::Component>> m_childs;
        
        private:

            virtual void draw(sf::RenderTarget& target , sf::RenderStates& states) const;

    };
}

#endif // COMPONENT_HPP