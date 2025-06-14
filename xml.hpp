#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#define SPACES "  "
#define newl "\n"

class Node {
public:
    Node(std::string);

    bool self_closing = false;
    std::string tag;
    std::map<std::string, std::string> attributes;
    std::vector<Node> children;
    std::string content; // textContent, ignored if there are any children
};

Node::Node(std::string tag_name) {
    tag = tag_name;
}

class XML {
public:
    XML() : root("root") {}

    void set_root(Node node);
    void print();
    void save(std::string fpath);

private:
    Node root;
    std::string buffer = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
    void generate();
    void generate(std::vector<Node> nodes);
    void open_tag(Node node);
    void write_content(Node node);
    void close_tag(Node node);
    unsigned int indent = 0;
};

void XML::set_root(Node node) {
    root = node;
}

void XML::save(std::string fpath) {
    std::ofstream ofs(fpath);
    ofs << buffer;
    ofs.close();
}

void XML::print() {
    generate();
    std::cout << buffer << newl;
}

void XML::generate() {
    buffer.clear();
    buffer = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
    generate({root});
}

void XML::generate(std::vector<Node> nodes) {
    for (size_t i = 0; i < nodes.size(); i++) {
        Node cur_node = nodes.at(i);

        open_tag(cur_node);
        if (cur_node.self_closing) {
            continue;
        }

        if (cur_node.children.size() > 0) {
            generate(cur_node.children);
        } else {
            // If there are no children, there might be content to print
            if (cur_node.content != "") {
                write_content(cur_node);
            }
            // Otherwise it's an empty node
        }
        close_tag(cur_node);
    }
}

void XML::open_tag(Node node) {
    for (size_t i = 0; i < indent; i++) {
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
        indent++;
    }
    
}

void XML::close_tag(Node node) {
    if (node.self_closing) {
        std::cerr << "Error: not supposed to close self closing tag" << newl;
        return;
    }

    indent--;
    for (size_t i = 0; i < indent; i++) {
        buffer += SPACES;
    }
    buffer += "</";
    buffer += node.tag;
    buffer += ">\n";
}

void XML::write_content(Node node) {
    for (size_t i = 0; i < indent; i++) {
        buffer += SPACES;
    }
    buffer += node.content;
    buffer += newl;
}
