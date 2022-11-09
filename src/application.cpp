#include "header/application.hpp"
#include "header/entry.hpp"

std::vector<std::string> split(std::string str , std::string delimiter){
    std::vector<std::string> result{};
    
    int eindex = 0;
    int bindex = 0;
    while(str.find(delimiter , eindex) != -1){
        eindex = str.find(delimiter , eindex)+1;
        result.push_back(str.substr(bindex , eindex-bindex-1));
        bindex = eindex;
    }

    if(result.empty()){
        result.push_back(str);
    } else {
        result.push_back(str.substr(bindex , str.length()-bindex));
    }
    
    return result;
    
}

ui::Application::Application(sf::Vector2f winSize , ui::Direction direction) : ui::Section(winSize , direction){
}

bool ui::Application::events(sf::Event & event , sf::Window & window){
    ui::Section::events(event , window);

    if(event.type == sf::Event::Resized){
        this->resize(sf::Vector2f{(float) event.size.width , (float) event.size.height});
    }

    return false;
}

bool ui::Application::load(Tree & tree , std::unordered_map<std::string , std::function<void()>> & events){

    auto treeStruct = tree.postfixeParcours();
    std::vector<std::shared_ptr<ui::Component>> tempCompList;
    std::string something {};

    for (auto & node : treeStruct){
        if(node->tag == "button"){
            std::shared_ptr<ui::Button> button = std::make_shared<ui::Button>(sf::Vector2f{50 , 50} , "");

            if(!node->orphean){
                std::cout << "XML build failed , button tag must be orphean" << std::endl;
                return false;
            }
            else {
                for(auto & attribute : node->attributes){
                    if(attribute.first == "text"){
                        button->setText(attribute.second);
                    }
                    else if (attribute.first == "text-size"){
                        button->setTextCharacterSize(std::stoi(attribute.second));
                    }
                    else if (attribute.first == "colors"){
                        auto colorStr = split(attribute.second , ";");
                        if(std::size(colorStr) != 3)
                            continue;

                        std::array<sf::Color , 3> colors {};

                        int i = 0;
                        for (auto cstr : colorStr){
                            sf::Color color;
                            auto rgba = split(cstr , ",");

                            if(std::size(rgba) < 3)
                                break;

                            color.r = std::stoi(rgba[0]);
                            color.g = std::stoi(rgba[1]);
                            color.b = std::stoi(rgba[2]);

                            if(std::size(rgba) == 4)
                                color.a = std::stoi(rgba[3]);
                            
                            colors[i] = color;
                            i++;
                        }

                        if(i != 3){
                            continue;
                        }
                        
                        button->set_colors(colors);
                    } else if (attribute.first == "event"){
                        if((events.find(attribute.second)) != events.end()){
                            button->target = events[attribute.second];
                        }
                    }
                }
            }
            tempCompList.emplace_back(std::static_pointer_cast<ui::Component>(button));
        }
        else if (node->tag == "textbox"){
            std::shared_ptr<ui::TextBox> textbox = std::make_shared<ui::TextBox>(sf::Vector2f{50 , 50});

            if(!node->orphean){
                std::cout << "XML build failed , entry tag must be orphean" << std::endl;
                return false;
            }

            tempCompList.emplace_back(std::static_pointer_cast<ui::Component>(textbox));
        }
        else if (node->tag == "entry"){
            std::shared_ptr<ui::Entry> entry = std::make_shared<ui::Entry>(sf::Vector2f{50 , 50});

            if(!node->orphean){
                std::cout << "XML build failed , entry tag must be orphean" << std::endl;
                return false;
            }

            tempCompList.emplace_back(std::static_pointer_cast<ui::Component>(entry));
        }
        else if (node->tag == "section" || node->tag == "application"){
            std::shared_ptr<ui::Section> section = std::make_shared<ui::Section>(sf::Vector2f{50 , 50});
            std::vector<float> weights;

            if(node->orphean){
                std::cout << "XML build failed , section tag must not be orphean" << std::endl;
            }

            for(auto & attribute : node->attributes){
                if(attribute.first == "color"){
                    sf::Color color;
                    auto rgba = split(attribute.second , ",");

                    if(std::size(rgba) < 3)
                        continue;

                    color.r = std::stoi(rgba[0]);
                    color.g = std::stoi(rgba[1]);
                    color.b = std::stoi(rgba[2]);

                    if(std::size(rgba) == 4)
                        color.a = std::stoi(rgba[3]);
                    
                    if(node->tag == "section"){
                        section->set_color(color);
                    } else {
                        set_color(color);
                    }
                    
                }
                else if (attribute.first == "weights"){
                    auto w = split(attribute.second , ";");
                    if(!weights.empty())
                        continue;
                    for(auto & val : w){
                        weights.push_back(std::stof(val));
                    }
                }
                else if (attribute.first == "flex-direction"){
                    if(node->tag == "section"){
                        if(attribute.second == "horizontal"){
                        section->setFlexDirection(ui::Direction::Horizontal);
                    }
                    else {
                        section->setFlexDirection(ui::Direction::Vertical);
                    }
                    } else {
                        if(attribute.second == "horizontal"){
                        setFlexDirection(ui::Direction::Horizontal);
                    }
                    else {
                        setFlexDirection(ui::Direction::Vertical);
                    }
                    }
                }
            }
            
            auto cn = node->getChildNumber();

            if(cn != 0 && std::size(tempCompList) >= cn){

                std::vector<std::shared_ptr<ui::Component>> children;
    
                for(int i = cn ; i > 0;i--){
                    children.emplace_back(tempCompList.back());
                    tempCompList.pop_back();
                }

                for(int j = std::size(children)-1 ; j >= 0 ; j--){
                    if(node->tag == "section"){
                        section->addChild(children[j]);
                    }
                    else {
                        addChild(children[j]);
                    }
                }

                if(node->tag == "section"){
                    section->setWeights(weights);
                } else {
                    setWeights(weights);
                }

            }

            if(node->tag == "section"){
                tempCompList.emplace_back(std::static_pointer_cast<ui::Component>(section));
            }
        }
    }

    // std::cout << "from here" << std::endl;
    // addChild(std::move(tempCompList[0]));
    return true;
}