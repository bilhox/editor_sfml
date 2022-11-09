#include "header/xml_parser.hpp"
#include <iostream>
#include <tuple>
#include <stack>

std::string strip(std::string str , bool before = true , bool after = true){
    
    if(before){
        auto i {std::find_if_not(std::begin(str), std::end(str), isspace)};
        str.erase(std::begin(str) , i);
    }

    if(after){
        std::reverse(std::begin(str) , std::end(str));
        auto h = std::find_if_not(std::begin(str), std::end(str), isspace);
        str.erase(std::begin(str) , h);
        std::reverse(std::begin(str) , std::end(str));
    }

    return str;
    
}

void Tree::updatePointers(){
    if(std::size(children) != 0){
        for(auto & child : children){
            child.parent = this;
            child.updatePointers();
        }
    }
}

std::tuple<std::string , std::map<std::string , std::string>> extract_datas(std::string str){

    if (str.find(" ") == -1){
        if(str[str.length()-1] == '/')
            str.erase(str.length()-1);
        return {str , {}};
    }
    else {
        std::string tag = str.substr(0,str.find(" "));
        int index = str.find(" ");

        std::map<std::string , std::string> attributes;

        while (index != -1){
            
            int eindex = str.find('=' , index);
            std::string key = str.substr(index , eindex-index);

            std::string value;

            int quote = str.find('"' , index);
            int apostrophe = str.find("'" , index);

            if ((apostrophe == -1) or ((quote != -1) and (quote < apostrophe))){
                value = str.substr(quote+1 , str.find('"' , quote+1)-1-quote);
                index = str.find(' ' , str.find('"' , quote+1));
            }
            else {
                value = str.substr(apostrophe+1 , str.find("'" , apostrophe+1)-1-apostrophe);
                index = str.find(' ' , str.find("'" , apostrophe+1));
            }

            attributes.insert({strip(key) , value});
        
        }

        return {tag , attributes};
    }

}

bool Tree::isEmpty() const{
    return (std::size(children) == 0);
}

std::vector<const Tree*> Tree::postfixeParcours() const{
    std::vector<const Tree*> list;
    if(std::size(children) != 0){
        for(auto & child : children){
            auto clist = child.postfixeParcours();
            list.insert(list.end() , clist.begin() , clist.end());
        }
    }
    list.push_back(this);
    return list;
}

unsigned int Tree::getChildNumber() const {
    return std::size(children);
}

Tree::Tree(std::string tag , std::string content){
    this->tag = tag;
    this->content = content;
}

void Tree::addchild(Tree& t){
    this->children.push_back(t);
}

Tree& Tree::operator[] (std::size_t idx){
    return this->children[idx];
}

const std::vector<Tree> Tree::get_childs() const{
    return this->children;
}

Tree parse(const char* file){

    std::ifstream fs {file};
    // File content
    std::string lines;
    // content of one specific line
    std::string line;

    // getting the whole file content
    while(std::getline(fs , line)){
        lines += line;
    }

    int sindex = 0;
    std::vector<std::string> parts;
    std::vector<std::string> contents;

    // cutting every part like : [tag,content,tag,content ...]
    while(sindex != -1){
        
        auto findex = lines.find('>' , sindex);
        parts.push_back(lines.substr(sindex+1 , findex-1-sindex));
        sindex = lines.find('<',sindex+1);
        if (sindex != -1){
            contents.push_back(strip(lines.substr(findex+1 , sindex-1-findex) , true , false));
        }
    }

    std::vector<unsigned int> node_heights;
    std::vector<Tree> tree_struct;

    unsigned int index {0};


    for (auto &part : parts){
        if(part.at(0) == '/'){
            index--;
            continue;
        }
        node_heights.push_back(index);

        if(part.at(part.size()-1) != '/'){
            index++;
        }
        
    }

    for (auto &part : parts){
        if(tree_struct.empty()){
            auto [tag , attributes] = extract_datas(part);
            Tree tree {tag , ""};
            tree.attributes = attributes;
            tree_struct.push_back(tree);
        }
        else{
            if(part[0] == '/'){
                continue;
            }

            auto [tag , attributes] = extract_datas(part);
            Tree child {tag , ""};
            child.attributes = attributes;

            if(part[part.length()-1] == '/'){
                child.orphean = true;
            }

            tree_struct.push_back(child);
        }
        
    }

    // packing contents
    
    std::stack<Tree*> treeTracker;
    int tindex = 0;
    for (int i = 0; i < std::size(parts)-1;i++){
        if(parts[i][parts[i].length()-1] != '/'){
            if(parts[i][0] == '/'){
                treeTracker.pop();
                treeTracker.top()->content += contents[i];
                continue;
            }
            Tree * tree = &tree_struct[tindex];
            treeTracker.push(tree);
            tree->content += contents[i];
            tindex ++;
        }
        else {
            treeTracker.top()->content += contents[i];
        }
    }

    Tree final_tree{"",""};
    unsigned int hindex {*std::max_element(node_heights.begin() , node_heights.end())};
    auto findex {std::begin(node_heights)};

    while(hindex != 0){
        findex = std::find(std::begin(node_heights) , std::end(node_heights),hindex);
        if(findex != std::end(node_heights)){
            int dist = std::distance(std::begin(node_heights),findex);
            auto tindex {tree_struct.begin()+dist-1};
            (*tindex).addchild(*(tindex+1));
            tindex++;
            node_heights.erase(findex);
            tree_struct.erase(tindex);
        }
        else{
            hindex--;
        }
    }

    final_tree = tree_struct[0];
    final_tree.updatePointers();
    return final_tree;
}