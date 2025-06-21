## [WIP] Simple XML Library

Generates XML files. That's it.

### Structure

The library manages `XML::Node` objects (there is no single `XML` object). `XML::Node`s make up a tree structure that can be iterated through. Each `XML::Node` has private `prev` and `next` pointers that form a doubly linked list. They also have a private `parent` pointer that points to their parent `XML::Node` and a vector of `XML::Node`s to store their children. See `main.cpp` for a usage example.

### License

GNU General Public License version 3 or later.