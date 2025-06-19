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

#define SPACES "    "
#define newl "\n"

////////////////////////////
// Node class declaration //
////////////////////////////

class Node {
public:
    Node(std::string tag_name);
    bool operator==(const Node& n) const;
    bool operator!=(const Node& n) const;

    bool self_closing = false;
    std::string tag;
    std::map<std::string, std::string> attributes;
    std::string content; // textContent, ignored if there are any children

    void add_child(Node node);
    void print();
    void print_tree();
    void save(std::string fpath);
    void clear_children();
    size_t get_children_count();
    Node get_child(size_t i);

    class Iterator;
    Iterator begin();
    Iterator end();

private:
    Node* parent = nullptr;
    std::vector<Node> children;
    Node* next = nullptr;
    Node* prev = nullptr;
    std::string buffer;
    std::vector<int> pos;

    Node* traverse(Node* n);
    void generate(Node* n);
    void open_tag(Node n);
    void close_tag(Node n);
    void write_content(Node n);
    std::string get_pos();
    Node* last_node_pointer(Node* nodes);

    static std::shared_ptr<Node> dummy;
};

////////////////////////////////
// Iterator class declaration //
////////////////////////////////

class Node::Iterator {
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

////////////////////////////
// Node class definitions //
////////////////////////////

// Public functions

Node::Node(std::string tag_name) {
    tag = tag_name;
    pos = {-1};
}

bool Node::operator==(const Node& n) const {
    return pos == n.pos;
}

bool Node::operator!=(const Node& n) const {
    return pos != n.pos;
}

void Node::add_child(Node node) {
    Node* node_ptr = &node;
    node_ptr->prev = last_node_pointer(this);
    node_ptr->parent = this;
    children.push_back(*node_ptr);
}

void Node::print() {
    buffer.clear();
    traverse(this);
    generate(this);
    std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << newl;
    std::cout << buffer;
    std::cout << newl;
}

void Node::print_tree() {
    for (auto it = begin(); it != end(); ++it) {
        size_t indent = it->pos.size()-1;
        for (size_t i = 0; i < indent; i++) {
            std::cout << SPACES;
        }
        std::cout << it->tag;
        if (!it->children.empty()) {
            std::cout << ":" << newl;
        } else {
            std::cout << newl;
        }
    }
}

void Node::save(std::string fpath) {
    buffer.clear();
    traverse(this);
    generate(this);
    std::ofstream ofs(fpath);
    ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << newl;
    ofs << buffer;
    ofs.close();
    std::cout << "Saved " << fpath << newl;
}

void XML::Node::clear_children() {
    children.clear();
}

size_t XML::Node::get_children_count() {
    return children.size();
}

Node XML::Node::get_child(size_t i) {
    if (i < children.size()) {
        return children.at(i);
    }
    return Node("null");
}

Node::Iterator Node::begin() {
    pos = {0};
    traverse(this);
    return Iterator(this);
}

Node::Iterator Node::end() {
    return Iterator(Node::dummy.get());
}

// Private functions and variables

std::shared_ptr<Node> Node::dummy = std::make_shared<Node>("dummy");

Node* Node::traverse(Node* n) {
    if (n->prev != nullptr) {
        n->prev->next = n;
    }

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
        buffer += "/>";
        buffer += newl;
    } else {
        buffer += ">";
    }

    if (!(node.children.empty()) && !(node.self_closing)) {
        buffer += newl;
    }
    
}

void Node::close_tag(Node node) {
    if (node.self_closing) {
        std::cerr << "Error: not supposed to close self closing tag" << newl;
        return;
    }

    if (!node.children.empty()) {
        for (size_t i = 0; i < node.pos.size()-1; i++) {
            buffer += SPACES;
        }
    }
    buffer += "</";
    buffer += node.tag;
    buffer += ">\n";
}

void Node::write_content(Node node) {
    buffer += node.content;
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

Node* Node::last_node_pointer(Node* n) {
    if (n->children.empty()) {
        return n;
    }

    size_t last_i = n->children.size() - 1;
    return last_node_pointer(&(n->children.at(last_i)));
}

////////////////////////////////
// Iterator class definitions //
////////////////////////////////

Node::Iterator::Iterator(Node* n) : pointer(n) {}

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

bool Node::Iterator::operator==(const Node::Iterator& it) {
    return it.pointer->pos == pointer->pos;
}

bool Node::Iterator::operator!=(const Node::Iterator& it) {
    return it.pointer->pos != pointer->pos;
}

} // close namespace XML
#endif
