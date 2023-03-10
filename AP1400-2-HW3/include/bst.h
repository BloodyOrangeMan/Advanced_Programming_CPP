#ifndef BST_H
#define BST_H

#include <functional>
#include <iostream>
#include <queue>
#include <compare>
#include <iomanip> 

class BST
{
public:
    class Node;

    BST();
    BST(const BST& bst);
    BST(BST&& bst);
    ~BST();

    Node *&get_root();
    void bfs(std::function<void(Node *&node)> func) const;
    size_t length() const;
    bool add_node(int value);
    Node **find_node(int value);
    Node **find_parent(int value);
    Node **find_successor(int value);
    bool delete_node(int value);
    
    friend std::ostream &operator<<(std::ostream &os, const BST& bst);

private:
    Node *root;
};

class BST::Node
{
public:
    Node(int value, Node *left, Node *right);
    Node();
    Node(const Node &node);
    Node(Node &&node) noexcept;

    std::partial_ordering operator<=>(const int &_value) const { return value <=> _value; }
    bool operator==(const int &_value) const { return value == _value; }
    Node& operator=(const Node &node);
    Node& operator=(Node &&node) noexcept;


    friend std::ostream &operator<<(std::ostream &os, const BST::Node &node);
    int value;
    Node *left;
    Node *right;
};
#endif // BST_H