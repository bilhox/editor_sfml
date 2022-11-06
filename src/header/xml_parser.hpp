#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

#include <stack>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>

class Tree{

    public :

        Tree * parent = nullptr;
        std::string content;
        std::string tag;
        std::map<std::string , std::string> attributes;
        bool orphean {false};

        bool isEmpty() const;
        Tree(std::string tag , std::string content);
        void addchild(Tree& t);
        Tree& operator[](std::size_t idx);
        const std::vector<Tree> get_childs() const;
        unsigned int getChildNumber() const;
        std::vector<const Tree*> postfixeParcours() const;
        void updatePointers();
    
    private:

        std::vector<Tree> children;

};

Tree parse(const char* file);

#endif