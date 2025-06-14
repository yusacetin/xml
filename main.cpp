#include "xml.hpp"

#include <iostream>

int main() {
    XML xml;
    Node n1("N1");
    Node n2("N2");
    Node n3("N3");
    Node n4("N4");
    Node n5("N5");
    Node n6("N6");
    
    n6.self_closing = true;
    n6.attributes = {
        {"atkey", "atval"}
    };

    n5.attributes = {
        {"at5", "5val"}
    };
    n5.content = "this is what N5 says";

    n3.attributes = {
        {"key1", "value1"},
        {"key2", "value2"}
    };
    
    n4.children = {n5};
    n3.children = {n5, n6};
    n2.children = {n3, n4, n5};
    n1.children = {n2, n3};

    xml.set_root(n1);
    xml.print();
    xml.save("test.xml");

    return 0;
}