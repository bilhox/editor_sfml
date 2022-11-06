#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "section.hpp"
#include "xml_parser.hpp"
#include <functional>
#include <unordered_map> 

namespace ui {

    class Application : public ui::Section {

        public:

            Application(sf::Vector2f winSize , ui::Direction direction);
            bool events(sf::Event & event , sf::Window & window);
            bool load(Tree & tree , std::unordered_map<std::string , std::function<void()>> & events);
        
    };
}

#endif // APLLICATION_HPP