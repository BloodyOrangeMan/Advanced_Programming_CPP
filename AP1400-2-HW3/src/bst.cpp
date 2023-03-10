#include "bst.h"

BST::Node::Node(int value, Node *left, Node *right)
    : value(value), left(left), right(right)
{
}

std::ostream &operator<<(std::ostream &os, const BST::Node &node)
{
    os << &node << "\t=> value:" << node.value << "\t   ";
    os << "left:" << std::left;
    os.width(12);
    os << node.left << std::right;
    os.width(12);
    os << "right:" << std::left;
    os << node.right;
    return os;
}
BST::Node::Node() : value(0), left(nullptr), right(nullptr) {}

BST::Node::Node(const Node &node)
    : value(node.value), left(node.left), right(node.right)
{
}

BST::Node::Node(Node &&node) noexcept : value(node.value), left(node.left), right(node.right)
{
    node.left = nullptr;
    node.right = nullptr;
    node.value = 0;
}
BST::Node &BST::Node::operator=(Node &&node) noexcept
{
    if (this != &node)
    {
        value = node.value;
        left = node.left;
        right = node.right;
        node.right = nullptr;
        node.left = nullptr;
        node.value = 0;
    }
    return *this;
}

BST::Node &BST::Node::operator=(const BST::Node &node)
{
    if (this != &node)
    {
        value = node.value;
        left = node.left;
        right = node.right;
    }

    return *this;
}

BST::BST() : root(nullptr) {}

BST::BST(const BST &bst) : root(bst.root) {}

BST::BST(BST &&bst) : root(bst.root) { bst.root = nullptr; }

BST::~BST()
{
    std::vector<Node *> nodes;
    bfs([&nodes](BST::Node *&node)
        { nodes.push_back(node); });
    for (auto &node : nodes)
        delete node;
}

BST::Node *&BST::get_root()
{
    return root;
}

void BST::bfs(std::function<void(Node *&node)> func) const
{
    if (root == nullptr)
        return;
    std::queue<Node *> node_queue;
    node_queue.push(root);
    while (!node_queue.empty())
    {
        Node *&first_node = node_queue.front();
        func(first_node);
        if (first_node->left)
        {
            node_queue.push(first_node->left);
        }
        if (first_node->right)
        {
            node_queue.push(first_node->right);
        }
        node_queue.pop();
    }
}

size_t BST::length() const
{
    size_t length = 0;
    auto count = [&length](Node *&node)
    { length++; };
    bfs(count);
    return length;
}

BST::Node *add_node_helper(BST::Node *&node, int value, bool &is_success)
{
    if (!node)
    {
        return new BST::Node(value, nullptr, nullptr);
    }
    if (node->value == value)
    {
        is_success = false;
        return node;
    }
    else if (node->value < value)
    {
        node->right = add_node_helper(node->right, value, is_success);
    }
    else
    {
        node->left = add_node_helper(node->left, value, is_success);
    }
    return node;
}

bool BST::add_node(int value)
{
    bool is_success = true;
    root = add_node_helper(root, value, is_success);
    return is_success;
}

std::ostream &operator<<(std::ostream &os, const BST &bst)
{
    os << "*********************************************************************"
          "***********"
       << std::endl;
    auto print_bst = [&os](BST::Node *&node)
    { os << *node << std::endl; };
    bst.bfs(print_bst);
    os << "binary search tree size :" << bst.length() << std::endl;
    os << "*********************************************************************"
          "***********"
       << std::endl;
    return os;
}

BST::Node **find_node_helper(BST::Node *&node, const int &value)
{
    if (!node)
    {
        return nullptr;
    }
    if (node->value == value)
    {
        return &node;
    }
    else if (node->value < value)
    {
        return find_node_helper(node->right, value);
    }
    else
    {
        return find_node_helper(node->left, value);
    }
}

BST::Node **BST::find_node(int value)
{
    Node *node = root;
    while (node)
    {
        if (node->value == value)
        {
            Node **p = new Node *;
            *p = node;
            return p;
        }
        else if (node->value < value)
        {
            node = node->right;
        }
        else
        {
            node = node->left;
        }
    }
    return nullptr;
    // return find_node_helper(root, value);
}

BST::Node **BST::find_parent(int value)
{
    Node *node = root;
    Node **parent = new Node *;
    while (node)
    {
        if (node->value == value)
        {
            return parent;
        }
        else if (node->value < value)
        {
            *parent = node;
            node = node->right;
        }
        else
        {
            *parent = node;
            node = node->left;
        }
    }
    return nullptr;
}

BST::Node **BST::find_successor(int value)
{
    Node **node_ptr{find_node(value)};
    if (!node_ptr)
        return nullptr;
    if ((*node_ptr)->right)
    {
        *node_ptr = (*node_ptr)->right;
        while ((*node_ptr)->left)
        {
            *node_ptr = (*node_ptr)->left;
        }
        Node **p = new Node *;
        *p = *node_ptr;
        return p;
    }
    else
    {
        Node *curr = root;
        Node *succ = nullptr;
        while (curr->value != value)
        {
            if (curr->value > value)
            {
                succ = curr;
                curr = curr->left;
            }
            else
            {
                curr = curr->right;
            }
        }

        Node **p = new Node *;
        *p = succ;
        return p;
    }
}

bool BST::delete_node(int value)
{
    Node *node = root;
    Node **parent = new Node *;
    while (node)
    {
        if (node->value == value)
        {
            if (node->left && node->right)
            {
                Node **succ = find_successor(value);
                std::cout << **succ << std::endl;
                int temp = (*succ)->value;
                delete_node((*succ)->value);
                node->value = temp;
                std::cout << *node << std::endl;
            }
            else if (node->left || node->right)
            {
                if ((*parent)->value < value)
                {
                    if (node->left)
                    {
                        (*parent)->right = node->left;
                    }
                    else
                    {
                        (*parent)->right = node->right;
                    }
                }
                else
                {
                    if (node->left)
                    {
                        (*parent)->left = node->left;
                    }
                    else
                    {
                        (*parent)->left = node->right;
                    }
                }
                delete node;
            }
            else
            {
                if ((*parent)->value > value)
                {
                    (*parent)->left = nullptr;
                }
                else
                {
                    (*parent)->right = nullptr;
                }
                delete node;
            }
            return true;
        }
        else if (node->value < value)
        {
            *parent = node;
            node = node->right;
        }
        else
        {
            *parent = node;
            node = node->left;
        }
    }
    return false;
}