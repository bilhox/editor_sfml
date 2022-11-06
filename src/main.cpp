#include <iostream> 
#include <cmath>
#include <SFML/Graphics.hpp>
#include "header/button.hpp"
#include "header/entry.hpp"
#include "header/xml_parser.hpp"
#include "header/utils.hpp"
#include "header/scrollbar.hpp"
#include "header/section.hpp"
#include "header/application.hpp"
#include "header/events.hpp"
#include <random>
#include <sstream>
#include <memory>
#include <utility>

sf::Color color;

void foo(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution dist(1,10);

    color = sf::Color(128+sin(dist(gen))*128 , 128+(cos(dist(gen))/2.f)*128 , 128+128*sin(dist(gen))*cos(dist(gen)));
}

int main()
{
    auto tree = parse("./assets/this.xml");
    auto parcours = tree.postfixeParcours();


    sf::RenderWindow window(sf::VideoMode(800, 600), "Something" , sf::Style::Default);

    ui::Application application {sf::Vector2f{window.getSize()} , ui::Direction::Vertical};

    std::unordered_map<std::string , std::function<void()>> events;
    events["printSomething"] = std::bind(printSomething , &application);

    application.load(tree , events);

    sf::Clock clock;
    float dt = 0;

    float average{};
    int n_value{0};
    float fps {0};

    std::array<sf::Cursor , 3> cursors {};

    cursors[0].loadFromSystem(sf::Cursor::Arrow);
    cursors[1].loadFromSystem(sf::Cursor::Text);
    cursors[2].loadFromSystem(sf::Cursor::Hand);

    uint8_t cursor_index {0};

    while (window.isOpen())
    {
        
        dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        average += dt;
        n_value ++;

        if(n_value >= 50){
            n_value = 0;
            fps = 1.f/(average / 50.f);
            average = 0;
            std::ostringstream ss;
            ss << "FPS : " << fps;
            window.setTitle(ss.str());
        }

        sf::Event event;

        while(window.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    {sf::FloatRect visibleArea(0.f , 0.f , event.size.width , event.size.height);
                    window.setView(sf::View(visibleArea));}
                    break;
                default:
                    break;

            }

            application.events(event , window);
        }
        // updates 
        application.update(dt);

        auto mpos {sf::Mouse::getPosition(window)};

        window.clear(color);

        // draw + cursor
        bool cursor_set {false};
        switch(application.getCursorState(mpos)){
            case sf::Cursor::Hand:
                if(cursor_index != 2){
                    window.setMouseCursor(cursors[2]);
                    cursor_index = 2;
                    
                }
                cursor_set = true;
                break;
            
            case sf::Cursor::Text:
                if(cursor_index != 1){
                    window.setMouseCursor(cursors[1]);
                    cursor_index = 1;
                    
                }
                cursor_set = true;
                break;
            
            case sf::Cursor::Arrow:
                if(cursor_index != 0){
                    window.setMouseCursor(cursors[0]);
                    cursor_index = 0;
                }
                cursor_set = true;
                break;
            
            default:
                break;
        }

        application.display(window);

        if (!cursor_set && cursor_index != 0){
            window.setMouseCursor(cursors[0]);
            cursor_index = 0;
        }
        // window.draw(text_box);
        window.display();
    }

    return 0;
}