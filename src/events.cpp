#include "header/events.hpp"
#include "header/entry.hpp"
#include <fstream>
#include <string>
#include <regex>
#include <filesystem>
#include "header/utils.hpp"

std::filesystem::path file {""};

bool containsOnlyASCII(const std::string& filePath) {
  for (auto c: filePath) {
    if (static_cast<unsigned char>(c) > 127) {
      return false;
    }
  }
  return true;
}

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
        if(!containsOnlyASCII(line)){
            std::cout << "The file opening process stopped because it contains non ASCII characters\n"
                      << "Note that it'll be changed in the future" << std::endl;
            return;
        }
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
}

void deletePath(ui::Application * application){
    auto pathEntry = std::dynamic_pointer_cast<ui::Entry>((*(*(*application)[0])[0])[1]);
    pathEntry->setString("");
}

void run(ui::Application * application){

    if(file.empty() || file.extension().string() != ".py")
        return;

    auto cmd = "py \""+file.string()+"\"";
    const char * cmdCharL = cmd.c_str();
    popen(cmdCharL , "w");
}

void save(ui::Application * application){
    if(!std::filesystem::exists(file.string()))
        return;
    auto entry = std::dynamic_pointer_cast<ui::TextBox>((*application)[1]);
    auto content = entry->getString();
    std::ofstream fwritter {file.string()};

    for(auto & line : content){
        fwritter << line << std::endl;
    }

}