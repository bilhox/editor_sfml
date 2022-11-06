#ifndef SCROLLBAR_HPP
#define SCROLLBAR_HPP

#include "component.hpp"
#include "button.hpp"

namespace ui {

    enum Direction {
                Vertical,
                Horizontal
            };

    class ScrollBar : public ui::Component
        {

        public:

            ScrollBar(float length , ui::Direction direction);
            void update(float dt);
            bool events(sf::Event & event , sf::Window & window);
            void setBarLength(float bar_length);
            sf::Cursor::Type getCursorState(sf::Vector2i mpos);
            void setBarColor(sf::Color color);
            void resize(sf::Vector2f size);
            float getMovabilityDistance();
            float getRemainingMovabilityDistance();
            bool isBarVisible();
            void synchronizeBar(float length);
        
        private:

            ui::Direction direction;
            float length;

            bool bar_visible {false};
            sf::RectangleShape bar;
            bool bar_moved {false};
            float mouse_diff {0};
            float movability_distance {0};

            virtual void draw(sf::RenderTarget& target , sf::RenderStates states) const;

    };
}


#endif // SCROLLBAR_HPP