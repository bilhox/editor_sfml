#include "header/events.hpp"
#include "header/entry.hpp"
#include <fstream>
#include <string>

std::string currentFile = "";

void updateTBString(ui::Application * application){
    auto entry = std::dynamic_pointer_cast<ui::Entry>((*application)[1]);

    std::ifstream fs {"../program.py"};
    currentFile = "../program.py";
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

void run(ui::Application * application){

    if(currentFile == "")
        return;

    auto cmd = "py \""+currentFile+"\"";
    const char * cmdCharL = cmd.c_str();
    popen(cmdCharL , "w");
}

void save(ui::Application * application){
    if(currentFile == "")
        return;
    auto entry = std::dynamic_pointer_cast<ui::Entry>((*application)[1]);
    auto content = entry->getString();
    std::ofstream fwritter {currentFile};

    for(auto & line : content){
        fwritter << line << std::endl;
    }

}