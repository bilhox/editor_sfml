#include "header/entry.hpp"
#include <iostream>
#include "header/utils.hpp"
#include <cmath>

unsigned int get_realCursorPos(unsigned int cursor , std::string line){
    unsigned int cursor_pos{0};
    for (int i = 0; i < cursor ; i++){
        char c {line[i]};
        if (c == '\t'){
            cursor_pos+=4;
        }
        else{
            cursor_pos+=1;
        }
    }
    
    return cursor_pos;
}

long ui::TextBox::getContentSize(){
    return std::size(this->lines)*static_cast<int>(this->cursor_t.getSize().y);
}

sf::Cursor::Type ui::TextBox::getCursorState(sf::Vector2i mpos){

    sf::Vector2f fmpos {mpos};
    if(collide_pr(this->scrollbarY.get_rect() , fmpos-this->getPosition(true)))
        return this->scrollbarY.getCursorState(mpos);

    if(collide_pr(this->scrollbarX.get_rect() , fmpos-this->getPosition(true)))
        return this->scrollbarX.getCursorState(mpos);

    if(this->focus){
        return sf::Cursor::Text;
    }
    else {
        return sf::Cursor::Arrow;
    }
}

void ui::TextBox::set_visibleLines(){
    if(std::size(this->lines) == 0)
        return;
    
    // beginning
    unsigned int a{(unsigned long)(this->insideOffset.y)/(unsigned long)this->cursor_t.getSize().y};
    // ending
    unsigned int b{};
    // pixel length
    int length {2};

    if(a < std::size(this->lines)){
        for (int i = a ; i < std::size(this->lines);i++){
            length += this->cursor_t.getSize().y;

            if(length - this->insideOffset.y > this->entry_rect.height){
                b = i+1;
                break;
            }
        }

        if (b == 0)
            b = std::size(this->lines);

        this->visibility_y = {a ,b-a};
    }
    else {
        this->visibility_y = {0,0};
    }

}

void ui::TextBox::set_visibleLinePortion(unsigned int line){

    if(line > std::size(this->lines)-1)
        return;
    
    unsigned int charactersVisible{};
    // beginning
    unsigned int a{};
    // ending
    unsigned int b{};
    // pixel length
    int length {4};
    bool aset {false};

    for (int i = 0 ; i < std::size(this->lines[line]);i++){
        char l = this->lines[line][i];
        length += this->glyph_width * ((l != '\t')?1:4);
        if(length - this->insideOffset.x > 0 && !aset){
            a = i;
            aset = true;
        }

        if(length - this->insideOffset.x > this->entry_rect.width){
            b = i+1;
            break;
        }
    }
    if(!aset){
        this->visibility_x[line] = {0,0};
        return;
    }

    if (b == 0)
        b = std::size(this->lines[line]);

    charactersVisible = b - a;
    this->visibility_x[line] = {a , charactersVisible};

}

void ui::TextBox::draw(sf::RenderTarget& target , sf::RenderStates states) const {

    states.transform *= this->getTransform();
    states.texture = NULL;

    target.draw(this->m_vertices , states);


    if(!(std::size(this->lines) == 0)){
        auto text_render{this->text_renderer};
        text_render.setPosition(sf::Vector2f(4.f , 2+this->text_renderer.getLineSpacing())-this->insideOffset);
        text_render.move(0 , this->visibility_y[0]*this->cursor_t.getSize().y);
        for (int i = this->visibility_y[0];i < this->visibility_y[0]+this->visibility_y[1];i++){
            auto line = this->lines[i];
            text_render.setString(line);
            target.draw(text_render , states);
            text_render.move(0 , this->cursor_t.getSize().y);
        }
    }

    if(this->draw_cursor){
        target.draw(this->cursor_t , states);
    }
    target.draw(this->scrollbarY , states);
    target.draw(this->scrollbarX , states);
    // target.draw(this->borders , states);

}



ui::TextBox::TextBox(sf::Vector2f size){

    this->resize(size);
    this->focus = false;

    if(!this->font.loadFromFile("../assets/font/CourierPrime.ttf")){
        std::cout << "Failed to load font for ui::Entry::Entry object : " << &*this << std::endl;
    }

    m_minSize = sf::Vector2f(50,50);

    this->text_renderer.setFont(this->font);
    this->text_renderer.setFillColor(sf::Color::Black);
    this->text_renderer.setString("|)");
    this->text_renderer.setCharacterSize(20);
    this->text_renderer.setOrigin(0 , this->text_renderer.getLocalBounds().top);
    this->text_renderer.setPosition(4 , 2+this->text_renderer.getLineSpacing());

    this->text_renderer.setString("a");
    this->glyph_width = this->font.getGlyph('a' , 20 , false).bounds.width+this->text_renderer.getLetterSpacing();
    this->cursor_t.setSize(sf::Vector2f(1 , this->text_renderer.getCharacterSize()));
    this->text_renderer.setOutlineColor(sf::Color::Blue);
    this->text_renderer.setString("");
    this->cursor_state_change_timer = 0.f;
    this->cursor_state_change_time = .5f;
    this->draw_cursor = false;

    this->set_cursorPos();
    this->cursor_t.setFillColor(sf::Color::Black);

    this->cursor = {0 , 0};

    this->add_line("");
    // this->set_visibleLinePortion(0);

    this->scrollbarY.parent = &*this;
    this->scrollbarY.set_color({200, 200, 200});
    this->scrollbarY.setBarColor({120, 120, 120});

    this->scrollbarX.parent = &*this;
    this->scrollbarX.set_color({200, 200, 200});
    this->scrollbarX.setBarColor({120, 120, 120});
}

void ui::TextBox::resize(sf::Vector2f size){
    ui::Component::resize(size);
    this->entry_rect.width = size.x - 20;
    this->entry_rect.height = size.y - 20;
    this->scrollbarY.resize({20 , size.y-20});
    this->scrollbarY.setPosition({size.x-20 , 0});
    this->scrollbarX.resize({size.x-20 , 20});
    this->scrollbarX.setPosition({0 , size.y-20});
    this->updateScrollbar(ui::Direction::Horizontal);
    this->updateScrollbar(ui::Direction::Vertical);
    this->set_visibleLines();
    this->followCursor();
}

void ui::TextBox::set_cursorPos(){
    this->cursor_t.setPosition(
        sf::Vector2f(
            (std::size(this->lines) != 0)?get_realCursorPos(this->cursor[0],this->lines[this->cursor[1]])*this->glyph_width+4-this->insideOffset.x:4,
            this->cursor[1]*this->cursor_t.getSize().y-this->insideOffset.y+2
            )
        );
}

void ui::TextBox::updateScrollbar(ui::Direction direction){
    if(direction == ui::Direction::Vertical){
        float minBL = (this->entry_rect.height-20)/20;
        this->scrollbarY.setBarLength(minBL+((this->entry_rect.height-minBL) * this->entry_rect.height / this->content_size_y));
        if(this->scrollbarY.isBarVisible()){
            this->ratioY = (this->content_size_y-this->entry_rect.height)/this->scrollbarY.getMovabilityDistance();
        }
        else {
            this->ratioY = 0.f;
        }
        this->scrollbarY.synchronizeBar(this->insideOffset.y / (this->content_size_y - this->entry_rect.height) * this->scrollbarY.getMovabilityDistance());
    } else if (direction == ui::Direction::Horizontal){
        float minBL = (this->entry_rect.width-20)/20;
        this->scrollbarX.setBarLength(minBL+((this->entry_rect.width-minBL) * this->entry_rect.width / this->content_size_x));
        if(this->scrollbarX.isBarVisible()){
            this->ratioX = (this->content_size_x-this->entry_rect.width)/this->scrollbarX.getMovabilityDistance();
        }
        else {
            this->ratioX = 0.f;
        }
        this->scrollbarX.synchronizeBar(this->insideOffset.x / (this->content_size_x - this->entry_rect.width) * this->scrollbarX.getMovabilityDistance());
    }
}

void ui::TextBox::remove_line(unsigned int index){
    if (index >= 0 && index < std::size(this->lines)){
        this->lines.erase(this->lines.begin()+index);
        this->set_visibleLines();
        this->content_size_y -= static_cast<int>(this->cursor_t.getSize().y);
        this->updateScrollbar(ui::Direction::Vertical);
    }
}

void ui::TextBox::add_line(std::string line , int index){
    if (index < 0){
        this->lines.push_back(line);
    } else {
        this->lines.insert(this->lines.begin()+index , line);
    }
    this->content_size_y += static_cast<int>(this->cursor_t.getSize().y);
    this->updateScrollbar(ui::Direction::Vertical);
    this->set_visibleLines();
}

unsigned long ui::TextBox::getMaxLineWidth(){

    unsigned long maxWidth = 0;
    for (auto & line : this->lines){
        if (std::size(line) > maxWidth)
            maxWidth = std::size(line);
    }

    return 4+maxWidth*this->glyph_width;
}

void ui::TextBox::followCursor(){
    // Following cursor
    if(this->entry_rect.width-(this->cursor[0]*this->glyph_width+4-this->insideOffset.x) < 0){
        this->insideOffset.x += this->cursor[0]*this->glyph_width+4-this->insideOffset.x-this->entry_rect.width;
        this->scrollbarX.synchronizeBar(this->insideOffset.x / (this->content_size_x - this->entry_rect.width) * this->scrollbarX.getMovabilityDistance());
    }
    else if (this->cursor[0]*this->glyph_width+4-this->insideOffset.x < 0){
        this->insideOffset.x = this->cursor[0]*this->glyph_width+4;
        this->scrollbarX.synchronizeBar(this->insideOffset.x / (this->content_size_x - this->entry_rect.width) * this->scrollbarX.getMovabilityDistance());
    }
    float cursorSize = this->cursor_t.getSize().y;
    if(this->entry_rect.height-(this->cursor[1]*(cursorSize+1)+2-this->insideOffset.y) < 0){
        this->insideOffset.y += this->cursor[1]*(cursorSize+1)+2-this->insideOffset.y-this->entry_rect.height;
        this->scrollbarY.synchronizeBar(this->insideOffset.y / (this->content_size_y - this->entry_rect.height) * this->scrollbarY.getMovabilityDistance());
    }
    else if (this->cursor[1]*cursorSize+2-this->insideOffset.y < 0){
        this->insideOffset.y = this->cursor[1]*cursorSize+2;
        this->scrollbarY.synchronizeBar(this->insideOffset.y / (this->content_size_y - this->entry_rect.height) * this->scrollbarY.getMovabilityDistance());
    }

    this->set_visibleLines();
    this->set_cursorPos();
}

bool ui::TextBox::events(sf::Event & event , sf::Window& window){
    
    if(this->scrollbarY.events(event , window) || this->scrollbarX.events(event , window))
        return false;

    sf::Vector2f mpos {sf::Mouse::getPosition(window)};
    if(this->parent){
        mpos -= this->parent->getPosition(true);
    }

    switch(event.type){

        case sf::Event::MouseWheelScrolled:
            
            if(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel){
                if(this->focus){
                    float wheelMov {event.mouseWheelScroll.delta};

                    if(wheelMov == -1){
                        this->insideOffset.y += this->cursor_t.getSize().y;
                        if(this->insideOffset.y > this->content_size_y-this->entry_rect.height)
                            this->insideOffset.y = this->scrollbarY.getMovabilityDistance()*this->ratioY;
                        
                    }
                    else {
                        this->insideOffset.y -= this->cursor_t.getSize().y;
                        if(this->insideOffset.y < 0)
                            this->insideOffset.y = 0;
                    }
                    this->scrollbarY.synchronizeBar(this->insideOffset.y / (this->content_size_y - this->entry_rect.height) * this->scrollbarY.getMovabilityDistance());
                    this->set_visibleLines();
                    this->set_cursorPos();
                    return true;
                }

            }
            break;

        case sf::Event::MouseButtonPressed:

            this->focus = collide_pr(this->rect , mpos);
            this->draw_cursor = this->focus;

            mpos -= this->getPosition(false);

            if (this->focus){
                auto mpos_offset {(mpos + this->insideOffset)};
                int l {};
                bool cursor_y {false};
                for (int i = this->visibility_y[0];i < this->visibility_y[0]+this->visibility_y[1] ; i++){
                    if(mpos_offset.y < (i+1)*this->cursor_t.getSize().y){
                        this->cursor[1] = i;
                        l = i;
                        cursor_y = true;
                        break;
                    }
                }

                if (!cursor_y){
                    this->cursor[1] = std::size(this->lines)-1;
                    l = std::size(this->lines)-1;
                }

                bool cursor_x{false};

                for(int i = 0 ; i <= std::size(this->lines[this->cursor[1]]);i++){
                    if(i*this->glyph_width+4 >= mpos_offset.x){
                        if(mpos_offset.x-((i-1)*this->glyph_width+4) < i*this->glyph_width+4-mpos_offset.x){
                            this->cursor[0] = i-1;
                        }
                        else {
                            this->cursor[0] = i;
                        }
                        cursor_x = true;
                        break;
                    }
                }

                if (!cursor_x){
                    this->cursor[0] = std::size(this->lines[l]);
                }

                this->set_cursorPos();
                return true;
            }

            break;
        
        case sf::Event::KeyPressed:

            if(this->focus){
                if(event.key.code == sf::Keyboard::Right){
                    if (std::size(this->lines) != 0){
                        if (this->cursor[0]+1 <= std::size(this->lines[this->cursor[1]])){
                            this->cursor[0] += 1;
                        } else if (std::size(this->lines)-1 > this->cursor[1]){
                            this->cursor[0] = 0;
                            this->cursor[1] += 1;
                        }
                    }
                    this->set_cursorPos();
                    this->followCursor();
                } else if (event.key.code == sf::Keyboard::Left){
                    if (this->cursor[0] > 0){
                        this->cursor[0] -= 1;
                    } else if (this->cursor[1] > 0){
                        this->cursor[1] -= 1;
                        this->cursor[0] = this->lines[this->cursor[1]].size();
                    }
                    this->followCursor();
                    this->set_cursorPos();
                } else if (event.key.code == sf::Keyboard::Up){
                    if(this->cursor[1] != 0){
                        this->cursor[1] --;
                        if (std::size(this->lines[this->cursor[1]]) < this->cursor[0]){
                            this->cursor[0] = std::size(this->lines[this->cursor[1]]);
                        }
                        this->set_cursorPos();
                        this->followCursor();
                    }
                } else if (event.key.code == sf::Keyboard::Down){
                    if(this->cursor[1] < std::size(this->lines)-1){
                        this->cursor[1] ++;
                        if (std::size(this->lines[this->cursor[1]]) < this->cursor[0]){
                            this->cursor[0] = std::size(this->lines[this->cursor[1]]);
                        }
                        this->set_cursorPos();
                        this->followCursor();
                    }
                }
                this->draw_cursor = true;
                this->cursor_state_change_timer = 0.f;
                return true;
            }
            break;
        
        case sf::Event::TextEntered:

            if(this->focus){
                char c {static_cast<char>(event.text.unicode)};

                if(event.text.unicode < 128){

                    if(event.text.unicode == '\b'){
                        if(std::size(this->lines) != 0){
                            if(std::size(this->lines[this->cursor[1]]) != 0){
                                if(this->cursor[0] != 0){
                                    this->lines[this->cursor[1]].erase(this->cursor[0]-1 , 1);
                                    this->cursor[0] --;
                                    // Update Horizontal scrollbar
                                    if ((std::size(this->lines[this->cursor[1]])+1)*this->glyph_width+4 >= this->content_size_x && this->content_size_x != this->getMaxLineWidth()){
                                        this->content_size_x -= this->glyph_width;
                                        this->updateScrollbar(ui::Direction::Horizontal);
                                    }
                                    // this->set_visibleLinePortion(this->cursor[1]);
                                } else if (this->cursor[1] != 0){
                                    this->cursor[0] = std::size(this->lines[this->cursor[1]-1]);
                                    this->lines[this->cursor[1]-1] += this->lines[this->cursor[1]];
                                    this->remove_line(this->cursor[1]);
                                    this->cursor[1] --;
                                    // Update Horizontal scrollbar
                                    if ((std::size(this->lines[this->cursor[1]]))*this->glyph_width+4 > this->content_size_x){
                                        this->content_size_x = (std::size(this->lines[this->cursor[1]]))*this->glyph_width+4;
                                        this->updateScrollbar(ui::Direction::Horizontal);
                                    }
                                    // this->set_visibleLinePortion(this->cursor[1]);
                                }
                                this->set_cursorPos();
                            } else if (this->cursor[1] != 0){
                                this->remove_line(this->cursor[1]);
                                this->cursor[1] -= 1;
                                this->cursor[0] = std::size(this->lines[this->cursor[1]]);
                                this->set_cursorPos();
                            }
                        }
                        this->followCursor();
                    }
                    else if (event.text.unicode == '\r'){
                        std::string new_line {};

                        if(this->cursor[0] < std::size(this->lines[this->cursor[1]])){
                            new_line = this->lines[this->cursor[1]].substr(this->cursor[0]);
                            this->lines[this->cursor[1]].erase(this->lines[this->cursor[1]].begin()+this->cursor[0] , this->lines[this->cursor[1]].end());
                        }

                        
                        if(this->cursor[1] < std::size(this->lines)-1){
                            this->add_line(new_line , this->cursor[1]+1);
                        }
                        else {
                            this->add_line(new_line);
                        }

                        // this->set_visibleLinePortion(this->cursor[1]+1);

                        this->cursor[1] += 1;
                        this->cursor[0] = 0;
                        this->set_cursorPos();
                        this->followCursor();
                    }
                    else if(event.text.unicode == '\t' || std::isprint(event.text.unicode)){
                        if(event.text.unicode != '\t'){
                            this->lines[this->cursor[1]].insert((this->cursor[0]) , {c});
                            this->cursor[0] += 1;
                        }
                        else {
                            this->lines[this->cursor[1]].insert((this->cursor[0]) , "    ");
                            this->cursor[0] += 4;
                        }

                        // Update Horizontal scrollbar
                        if ((std::size(this->lines[this->cursor[1]]))*this->glyph_width+4 > this->content_size_x){
                            this->content_size_x = (std::size(this->lines[this->cursor[1]]))*this->glyph_width+4;
                            this->updateScrollbar(ui::Direction::Horizontal);
                        }

                        this->followCursor();


                        this->set_cursorPos();
                    }

                    this->draw_cursor = true;
                    this->cursor_state_change_timer = 0.f;
                }
                return true;
            }

            break;

        default:
            break;
    }
    return false;

}

void ui::TextBox::update(float dt){

    if((this->focus)){
        this->cursor_state_change_timer += dt;

        if(this->cursor_state_change_timer - this->cursor_state_change_time >= 0){
            this->cursor_state_change_timer = 0.f;
            this->draw_cursor = !(this->draw_cursor);
        }
    }

    if(this->scrollbarY.isBarVisible()){
        float supposed_offset = this->ratioY*this->scrollbarY.getRemainingMovabilityDistance();
        if(this->insideOffset.y != supposed_offset){
            this->insideOffset.y = supposed_offset;
            this->set_visibleLines();
            this->set_cursorPos();
        }
    }
    else if(this->insideOffset.y != 0){

        this->insideOffset.y = 0;

    }

    if(this->scrollbarX.isBarVisible()){
        float supposed_offset = this->ratioX*this->scrollbarX.getRemainingMovabilityDistance();
        if(this->insideOffset.x != supposed_offset){
            this->insideOffset.x = supposed_offset;
            // for(int i = this->visibility_y[0];i < this->visibility_y[0]+this->visibility_y[1];i++)
            //     this->set_visibleLinePortion(i);
            this->set_cursorPos();
        }
    }
    else if(this->insideOffset.x != 0){

        this->insideOffset.x = 0;

    }
}

void ui::TextBox::setString(std::vector<std::string> & lines){
    this->lines.clear();
    this->lines = lines;
    this->cursor[0] = 0;
    this->cursor[1] = 0;
    content_size_x = this->getMaxLineWidth();
    content_size_y = 4+std::size(lines)*cursor_t.getSize().y;
    this->set_visibleLines();
    this->set_cursorPos();
    this->updateScrollbar(ui::Direction::Horizontal);
    this->updateScrollbar(ui::Direction::Vertical);
}

const std::vector<std::string> ui::TextBox::getString() const {
    return lines;
}

/*

///////////////////////DEFINITION OF ENTRY CLASS METHODS///////////////////////////

*/

ui::Entry::Entry(sf::Vector2f size){

    this->resize(size);
    this->focus = false;

    if(!this->font.loadFromFile("../assets/font/CourierPrime.ttf")){
        std::cout << "Failed to load font for ui::Entry::Entry object : " << &*this << std::endl;
    }

    m_minSize = sf::Vector2f(20,20);

    this->set_color({100 , 100 , 100});
    this->text_renderer.setFont(this->font);
    this->text_renderer.setFillColor(sf::Color::White);
    this->text_renderer.setString("|)");
    this->text_renderer.setCharacterSize(20);
    this->text_renderer.setOrigin(0 , this->text_renderer.getLocalBounds().top);
    this->text_renderer.setPosition(4 , 2+this->text_renderer.getLineSpacing());

    this->glyph_width = this->font.getGlyph('a' , 20 , false).bounds.width+this->text_renderer.getLetterSpacing();
    this->cursor_t.setSize(sf::Vector2f(1 , this->text_renderer.getCharacterSize()));
    this->text_renderer.setOutlineColor(sf::Color::Blue);
    this->text_renderer.setString("");
    this->cursor_state_change_timer = 0.f;
    this->cursor_state_change_time = .5f;
    this->draw_cursor = false;

    this->set_cursorPos();
    this->cursor_t.setFillColor(sf::Color::White);

    this->cursor = 0;
}

void ui::Entry::update(float dt){

    if((this->focus)){
        this->cursor_state_change_timer += dt;

        if(this->cursor_state_change_timer - this->cursor_state_change_time >= 0){
            this->cursor_state_change_timer = 0.f;
            this->draw_cursor = !(this->draw_cursor);
        }
    }
}

void ui::Entry::draw(sf::RenderTarget & target , sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = NULL;

    target.draw(m_vertices , states);

    if(!(std::size(this->text) == 0)){
        auto text_render{this->text_renderer};
        text_render.setPosition(sf::Vector2f(2.f-this->insideOffset , rect.height/2-9));
        text_render.setString(this->text);
        target.draw(text_render , states);
    }

    if(this->draw_cursor){
        target.draw(this->cursor_t , states);
    }

}

bool ui::Entry::events(sf::Event & event , sf::Window & window){

    sf::Vector2f mpos {sf::Mouse::getPosition(window)};
    if(this->parent){
        mpos -= this->parent->getPosition(true);
    }

    switch(event.type){
        case sf::Event::MouseButtonPressed:

            this->focus = collide_pr(this->rect , mpos);
            this->draw_cursor = this->focus;

            // mpos -= this->getPosition(false);

            // if (this->focus){
            //     auto mpos_offset {mpos};
            //     mpos_offset.x -= this->insideOffset;

            //     bool cursor_x{false};

            //     // for(int i = 0 ; i <= std::size(this->text);i++){
            //     //     if(i*this->glyph_width+4 >= mpos_offset.x){
            //     //         if(mpos_offset.x-((i-1)*this->glyph_width+4) < i*this->glyph_width+4-mpos_offset.x){
            //     //             this->cursor = i-1;
            //     //         }
            //     //         else {
            //     //             this->cursor = i;
            //     //         }
            //     //         cursor_x = true;
            //     //         break;
            //     //     }
            //     // }

            //     // if (!cursor_x){
            //     //     this->cursor = std::size(this->text);
            //     // }

            // }
            this->set_cursorPos();
            break;
        
        case sf::Event::KeyPressed:
            if(this->focus){
                if(event.key.code == sf::Keyboard::Right){
                    if (this->cursor+1 <= std::size(this->text)){
                        this->cursor += 1;
                    } 
                    this->set_cursorPos();
                    this->followCursor();
                } else if (event.key.code == sf::Keyboard::Left){
                    if (this->cursor > 0){
                        this->cursor -= 1;
                    } 
                    this->followCursor();
                    this->set_cursorPos();
                }

                this->draw_cursor = true;
                this->cursor_state_change_timer = 0.f;
                return true;
            }
            break;
        
        case sf::Event::TextEntered:
            if(this->focus){
                char c {static_cast<char>(event.text.unicode)};

                if(event.text.unicode < 128){

                    if(event.text.unicode == '\b'){
                            if(std::size(this->text) != 0){
                                if(this->cursor > 0){
                                    this->text.erase(this->cursor-1 , 1);
                                    this->cursor --;
                                    this->content_size -= this->glyph_width;
                                }
                                this->set_cursorPos();
                            } 
                        this->followCursor();
                    }
                    else if(event.text.unicode == '\t' || std::isprint(event.text.unicode)){
                        if(event.text.unicode != '\t'){
                            this->text.insert((this->cursor) , {c});
                            this->cursor += 1;
                            this->content_size += this->glyph_width;
                        }
                        else {
                            this->text.insert(this->cursor , "    ");
                            this->cursor += 4;
                            this->content_size += this->glyph_width*4;
                        }

                        this->followCursor();


                        this->set_cursorPos();
                    }

                    this->draw_cursor = true;
                    this->cursor_state_change_timer = 0.f;
                }
                return true;
            }

            break;
    }

    return false;
    
}

void ui::Entry::set_cursorPos(){
    this->cursor_t.setPosition(sf::Vector2f(2+this->cursor*this->glyph_width-this->insideOffset , rect.height/2-cursor_t.getSize().y/2));
}

sf::Cursor::Type ui::Entry::getCursorState(sf::Vector2i mpos){

    if(this->focus){
        return sf::Cursor::Text;
    }
    else {
        return sf::Cursor::Arrow;
    }

}

void ui::Entry::setString(std::string text){
    this->text.clear();
    this->text.assign(text);
    this->cursor = 0;
    content_size = std::size(this->text)*this->glyph_width+2;
    this->set_cursorPos();
}

const std::string ui::Entry::getString() const {
    return text;
}

void ui::Entry::followCursor(){
    if(this->rect.width-(this->cursor*this->glyph_width+2-this->insideOffset) < 0){
        this->insideOffset += this->cursor*this->glyph_width+2-this->insideOffset-this->rect.width;
    }
    else if (this->cursor*this->glyph_width-this->insideOffset < 0){
        this->insideOffset = (this->insideOffset > this->rect.width)?(this->insideOffset-this->rect.width):0;
    }
    this->set_cursorPos();
}
