#include "header/events.hpp"
#include "header/entry.hpp"
#include <fstream>
#include <string>
#include <regex>
#include <filesystem>

std::filesystem::path file {""};

void openFile(ui::Application * application){
    auto entry = std::dynamic_pointer_cast<ui::TextBox>((*application)[1]);
    auto entryPath = std::dynamic_pointer_cast<ui::Entry>((*(*(*application)[0])[0])[1]);
    auto path = entryPath->getString();

    if(path == "")
        return;

    std::ifstream fs {path};

    if(!fs)
        return;

    file = std::filesystem::path(entryPath->getString());
    std::vector<std::string> lines;
    std::string line;

    while(std::getline(fs , line)){
        lines.push_back(line);
    }

    if(lines.empty()){
        std::cout << "File is empty , added extra line" << std::endl;
        lines.push_back("");
    }

    entry->setString(lines);
 
}

void pastePath(ui::Application * application){
    auto pathEntry = std::dynamic_pointer_cast<ui::Entry>((*(*(*application)[0])[0])[1]);

    std::string p = sf::Clipboard::getString();
    if(!std::filesystem::exists(p))
        return;
    
    std::string path = sf::Clipboard::getString();
    pathEntry->setString(path);

    // std::string path = sf::Clipboard::getString();
    // std::cout << path << std::endl;
    // std::regex pathRegex {"^(?:[A-Z]:|[.]{1,2})(\\[a-zA-Z\\\\s]+)+\\.(py|txt|js|cpp|hpp|c|html|css|h|xml|json)$"};
    // std::smatch match;
    // if(std::regex_match(path , match , pathRegex)){
    //     std::cout << "done" << std::endl;
    // }
}

void run(ui::Application * application){

    if(file.empty() || file.extension().string() != ".py")
        return;

    auto cmd = "py \""+file.string()+"\"";
    const char * cmdCharL = cmd.c_str();
    popen(cmdCharL , "w");
}

void save(ui::Application * application){
    if(file.empty())
        return;
    auto entry = std::dynamic_pointer_cast<ui::TextBox>((*application)[1]);
    auto content = entry->getString();
    std::ofstream fwritter {file.root_path().string()};

    for(auto & line : content){
        fwritter << line << std::endl;
    }

}