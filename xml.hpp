#ifndef XML_HPP
#define XML_HPP

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <memory>
#include <stack>

namespace XML {

#define SPACES "  "
#define newl "\n"

class Node {
public:
    Node(std::string tag_name);

    bool self_closing = false;
    std::string tag;
    std::map<std::string, std::string> attributes;
    std::string content; // textContent, ignored if there are any children

    void add_child(Node node);
    void print();
    void save(std::string fpath);

    class Iterator {
    public:
        Iterator(Node* n);
        Node operator*() const;
        Node* operator->();
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& it);
        bool operator!=(const Iterator& it);

    private:
        Node* pointer;
    };

    Iterator begin();
    Iterator end();
    bool operator==(const Node& n) const;
    bool operator!=(const Node& n) const;

private:
    Node* parent = nullptr;
    std::vector<Node> children;
    Node* next = nullptr;
    Node* prev = nullptr;

    Node* traverse(Node* n);
    std::vector<int> pos;
    std::string get_pos();
    Node* last_node_pointer(Node* nodes);
    void generate(Node* n);
    void open_tag(Node n);
    void close_tag(Node n);
    void write_content(Node n);

    static std::shared_ptr<Node> dummy;
    std::string buffer;
    
};

std::shared_ptr<Node> Node::dummy = std::make_shared<Node>("dummy");

Node::Node(std::string tag_name) {
    tag = tag_name;
    pos = {-1};
}

Node::Iterator::Iterator(Node* n) : pointer(n) {}

bool Node::operator==(const Node& n) const {
    return pos == n.pos;
}

bool Node::operator!=(const Node& n) const {
    return pos != n.pos;
}

Node* Node::last_node_pointer(Node* n) {
    if (n->children.empty()) {
        return n;
    }

    size_t last_i = n->children.size() - 1;
    return last_node_pointer(&(n->children.at(last_i)));
}

void Node::add_child(Node node) {
    Node* node_ptr = &node;
    node_ptr->prev = last_node_pointer(this);
    node_ptr->parent = this;
    children.push_back(*node_ptr);
}

Node::Iterator Node::begin() {
    pos = {0};
    traverse(this);
    return Iterator(this);
}

Node::Iterator Node::end() {
    return Iterator(Node::dummy.get());
}

bool Node::Iterator::operator==(const Node::Iterator& it) {
    return it.pointer->pos == pointer->pos;
}

bool Node::Iterator::operator!=(const Node::Iterator& it) {
    return it.pointer->pos != pointer->pos;
}

// This could be much more easily implemented using generate()
// but I specifically wanted to use iterators for fun
void Node::print() {
    std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << newl;
    std::stack<Node> nstack;
    for (auto it = begin(); it != end(); ++it) {
        // First, close any potential previously opened nodes
        if (!nstack.empty()) {
            if (it->pos.size() == nstack.top().pos.size()) {
                for (size_t i = 0; i < it->pos.size() - 1; i++) {
                    std::cout << SPACES;
                }
                Node n_top = nstack.top();
                std::cout << "</";
                std::cout << n_top.tag;
                std::cout << ">" << newl;

                nstack.pop();
            }
        }
        
        // Now begin writing current node

        for (size_t i = 0; i < it->pos.size() - 1; i++) {
            std::cout << SPACES;
        }

        std::cout << "<" << it->tag;
        for (auto it_attr = it->attributes.begin(); it_attr != it->attributes.end(); ++it_attr) {
            std::cout << " ";
            std::cout << it_attr->first;
            std::cout << "=";
            std::cout << "\"";
            std::cout << it_attr->second;
            std::cout << "\"";
        }

        if (it->self_closing) {
            std::cout << "/>\n";
        } else {
            std::cout << ">\n";
            if (it->children.empty()) {
                if (it->content != "") {
                    for (size_t i = 0; i < it->pos.size(); i++) {
                        std::cout << SPACES;
                    }
                    std::cout << it->content << newl;
                }
                
                for (size_t i = 0; i < it->pos.size() - 1; i++) {
                    std::cout << SPACES;
                }
                std::cout << "</";
                std::cout << it->tag;
                std::cout << ">" << newl;
            }
        }

        if (it->children.size() > 0) {
            nstack.push(*it);
        }
    }

    std::cout << "</";
    std::cout << tag;
    std::cout << ">" << newl;
}

Node Node::Iterator::operator*() const {
    return *pointer;
}

Node* Node::Iterator::operator->() {
    return pointer;
}

Node::Iterator& Node::Iterator::operator++() {
    if (pointer->next == nullptr) {
        pointer = Node::dummy.get();
        return *this;
    }
    pointer = pointer->next;
    return *this;
}

Node::Iterator Node::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this); // just redirect to prefix operator
    return temp;
}

Node* Node::traverse(Node* n) {
    if (n->prev != nullptr) {
        n->prev->next = n;
    }

    /*std::cout << newl;
    std::cout << "[DEBUG] current node is " << n->tag << ":" << n->get_pos() << newl;
    if (n->prev != nullptr) {
        std::cout << "[DEBUG] prev node is " << n->prev->tag << ":" << n->prev->get_pos() << newl;
        std::cout << "[DEBUG] next of prev node is " << n->prev->next->tag << ":" << n->prev->next->get_pos() << newl;
    } else {
        std::cout << "[DEBUG] current node does not have prev" << newl;
    }*/

    Node* n_last = n; // if there are no children, return self

    std::vector<int> new_pos = n->pos;
    if (n->children.size() > 0) {
        new_pos.push_back(0);
    }
    for (size_t i = 0; i < n->children.size(); i++) {
        Node* cur_child = &(n->children.at(i));
        cur_child->pos = new_pos;

        cur_child->prev = n_last;
        cur_child->prev->next = cur_child;

        n_last = traverse(cur_child);

        new_pos.at(new_pos.size()-1)++;
    }

    return n_last;
}

std::string Node::get_pos() {
    std::string s = "{";
    for (size_t i = 0; i < pos.size(); i++) {
        s += std::to_string(pos.at(i));
        if (i != pos.size()-1) {
            s += ",";
        }
    }
    s += "}";
    return s;
}

void Node::generate(Node* n) {
    open_tag(*n);

    std::vector<Node> n_children = n->children;
    for (size_t i = 0; i < n_children.size(); i++) {
        Node cur_node = n_children.at(i);
        generate(&cur_node);
    }

    if (!n->self_closing) {
        if (n_children.empty()) {
            if (n->content != "") {
                write_content(*n);
            }
        }
        close_tag(*n);
    }
}

void Node::open_tag(Node node) {
    for (size_t i = 0; i < node.pos.size()-1; i++) {
        buffer += SPACES;
    }
    buffer += "<";
    buffer += node.tag;

    for (auto it = node.attributes.begin(); it != node.attributes.end(); ++it) {
        buffer += " ";
        buffer += it->first;
        buffer += "=";
        buffer += "\"";
        buffer += it->second;
        buffer += "\"";
    }

    if (node.self_closing) {
        buffer += "/>\n";
    } else {
        buffer += ">\n";
    }
    
}

void Node::close_tag(Node node) {
    if (node.self_closing) {
        std::cerr << "Error: not supposed to close self closing tag" << newl;
        return;
    }

    for (size_t i = 0; i < node.pos.size()-1; i++) {
        buffer += SPACES;
    }
    buffer += "</";
    buffer += node.tag;
    buffer += ">\n";
}

void Node::write_content(Node node) {
    for (size_t i = 0; i < node.pos.size(); i++) {
        buffer += SPACES;
    }
    buffer += node.content;
    buffer += newl;
}

void Node::save(std::string fpath) {
    generate(this);
    std::ofstream ofs(fpath);
    ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << newl;
    ofs << buffer;
    ofs.close();
}

} // close namespace XML
#endif
