#ifndef SECTION_HPP
#define SECTION_HPP

#include "component.hpp"
#include "scrollbar.hpp"

namespace ui {
    
    class Section : public ui::Component{
        public:

            Section(sf::Vector2f size , ui::Direction flexDirection = ui::Vertical);
            void update(float dt);
            bool events(sf::Event& event , sf::Window& window);
            void display(sf::RenderWindow & window);
            void resize(sf::Vector2f size);
            void setWeights(std::vector<float> weights);
            void addChild(std::shared_ptr<ui::Component> component_ptr);
            void setFlexDirection(ui::Direction direction);
            sf::Cursor::Type getCursorState(sf::Vector2i mpos);
        
        private:

            ui::Direction m_flexDirection;
            std::vector<float> m_weights;

            void updateChildrenSize();
            virtual void draw(sf::RenderTarget& target , sf::RenderStates states) const;

    };
}

#endif // SECTION_HPP