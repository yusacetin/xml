/*
This file is part of Simple XML Library.

Simple XML Library is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Simple XML Library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Simple XML Library.
If not, see <https://www.gnu.org/licenses/>.
*/

#include "xml.hpp"

#include <iostream>

int main() {
    XML::Node n1("N1");
    XML::Node n2("N2");
    XML::Node n3("N3");
    XML::Node n4("N4");
    XML::Node n5("N5");
    XML::Node n6("N6");
    XML::Node n7("N7");
    
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

    n4.add_child(n5);
    
    n3.add_child(n5);
    n3.add_child(n6);

    n2.add_child(n3);
    n2.add_child(n4);
    n2.add_child(n5);

    n1.add_child(n2);
    n1.add_child(n3);
    n1.add_child(n7);

    n1.print_tree();
    n1.save("test.xml");

    return 0;
}