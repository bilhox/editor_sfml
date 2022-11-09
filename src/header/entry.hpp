#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <SFML/Graphics.hpp>
#include "component.hpp"
#include "scrollbar.hpp"
#include <string>
#include <vector>
#include <array>

namespace ui {

    class TextBox : public ui::Component{

    public:

        TextBox(sf::Vector2f size);
        void update(float dt);
        bool events(sf::Event& event , sf::Window& window);
        void resize(sf::Vector2f size);
        sf::Cursor::Type getCursorState(sf::Vector2i mpos);
        long getContentSize();
        void setString(std::vector<std::string> & lines);
        const std::vector<std::string> getString() const;

    private:

        sf::FloatRect entry_rect;

        sf::RectangleShape borders;
        bool focus;
        sf::Text text_renderer;
        std::vector<std::string> lines;
        sf::Font font;

        std::array<int , 2> cursor;
        bool draw_cursor;
        sf::RectangleShape cursor_t;

        float cursor_state_change_time;
        float cursor_state_change_timer;
        std::vector<std::array<unsigned int , 2>> visibility_x;
        std::array<unsigned int , 2> visibility_y;
        sf::Vector2f insideOffset;
        
        float glyph_width;

        ui::ScrollBar scrollbarY {10 , ui::Vertical};
        ui::ScrollBar scrollbarX {10 , ui::Horizontal};
        long long content_size_y {2};
        long long content_size_x {4};
        float ratioY;
        float ratioX;

        // So the entry camera follows the cursor
        void followCursor();
        // To add a line
        void add_line(std::string line , int index = -1);
        // To remove a line
        void remove_line(unsigned int index);
        // To set the interval of lines visible
        void set_visibleLines();
        // Not used due to some problems , it'll be back later
        void set_visibleLinePortion(unsigned int line);
        // set the texture cursor pos
        void set_cursorPos();
        // update the scrollbars
        void updateScrollbar(ui::Direction direction);
        // the method name
        unsigned long getMaxLineWidth();
        // To draw the component
        virtual void draw(sf::RenderTarget &target , sf::RenderStates states) const;
    };

    class Entry : public ui::Component {

        public :

            Entry(sf::Vector2f size);
            void update(float dt);
            bool events(sf::Event& event , sf::Window& window);
            sf::Cursor::Type getCursorState(sf::Vector2i mpos);
            void setString(std::string text);
            const std::string getString() const;
        
        private:

            sf::Text text_renderer;
            sf::Font font;

            std::string text;
            unsigned int maxLength;
            long long content_size {2};
            bool focus;
            float cursor_state_change_time;
            float cursor_state_change_timer;

            int cursor;
            bool draw_cursor;
            sf::RectangleShape cursor_t;
            float glyph_width;

            float insideOffset;

            void followCursor();

            // set the texture cursor pos
            void set_cursorPos();

            virtual void draw(sf::RenderTarget & target , sf::RenderStates states) const;
    };
}

#endif