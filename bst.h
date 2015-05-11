//
// Created by Placinta on 5/11/15.
//

#ifndef ALGS_BST_H
#define ALGS_BST_H

template <typename Key, typename Value>
class BST {
public:
    struct TreeNode;
    typedef std::shared_ptr<TreeNode> NodeP;
    typedef std::pair<Value, bool> MaybeValue;

    typedef struct TreeNode {
        TreeNode() : key(), value(), left(nullptr), right(nullptr), _size(0) {}
        TreeNode(Key _key, Value _val) : key(_key), value(_val), left(nullptr), right(nullptr), _size(1) {}
        TreeNode(Key _key, Value _val, NodeP _left, NodeP _right) : key(_key), value(_val), left(_left), right(_right), _size(0) {}

        bool operator==(const TreeNode& other) {
            return key == other.key;
        }

        bool operator==(const Key& other_key) {
            return key == other_key;
        }

        bool operator!=(const TreeNode& other) {
            return key != other.key;
        }

        bool operator<(const TreeNode& other) {
            return key < other.key;
        }

        bool operator<=(const TreeNode& other) {
            return key <= other.key;
        }

        bool operator>(const TreeNode& other) {
            return key > other.key;
        }

        bool operator>=(const TreeNode& other) {
            return key > other.key;
        }

        friend std::ostream& operator<<(std::ostream& os, const TreeNode& node) {
            os << "(" << node.key << ", " << node.value << ")";
            return os;
        }

        static void TreeNodeRemoveLog (TreeNode* removed_node) {
            std::cout << "Removing node " << *removed_node << std::endl;
            delete removed_node;
        }

        size_t size() { return _size; };
        void size(size_t size) { _size = size; };

        Key key;
        Value value;
        NodeP left;
        NodeP right;

    private:
        size_t _size;
    } TreeNode;

    BST() : root() {}

    void insert(Key key, Value val) {
        root = insert(root, key, val);
    }

    NodeP insert(NodeP node, Key key, Value val) {
        if (node == nullptr) {
            node = std::make_shared<TreeNode>(key, val);
            // node = std::shared_ptr<TreeNode>(new TreeNode(key, val), BST::TreeNode::TreeNodeRemoveLog);
            return node;
        }
        if (key < node->key) {
            node->left = insert(node->left, key, val);
        }
        else if (key > node->key) {
            node->right = insert(node->right, key, val);
        }
        else {
            node->value = val;
        }
        node->size(1 + size(node->left) + size(node->right));

        return node;
    }

    NodeP get(NodeP node, Key key) {
        if (node == nullptr) {
            return node;
        }

        if (key < node->key) {
            return get(node->left, key);
        }
        else if (key > node->key) {
            return get(node->right, key);
        }
        else return node;
    }

    MaybeValue get(Key key) {
        NodeP node = get(root, key);
        if (node != nullptr) {
            return std::make_pair(node->value, true);
        }
        else {
            return std::make_pair(Value(), false);
        }
    }

    void remove(Key key) {
        root = remove(root, key);
    }

    NodeP remove(NodeP node, Key key) {
        if (node == nullptr) return nullptr;
        if (key < node->key) {
            node->left = remove(node->left, key);
        }
        else if (key > node->key) {
            node->right = remove(node->right, key);
        }
        else {
            auto element_count = size(node);
            if (element_count == 1) {
                // No children case.
                return nullptr;
            }
            else if (element_count == 2) {
                // One child case.
                NodeP child = node->left;
                if (node->right != nullptr) {
                    child = node->right;
                }
                return child;
            }
            else {
                // Two children case.
                NodeP successor = findMinNode(node->right);
                successor->right = remove(node->right, successor->key);
                successor->left = node->left;
                node = successor;
            }
        }

        node->size(1 + size(node->left) + size(node->right));
        return node;
    }

    NodeP findMinNode(NodeP node) {
        if (node == nullptr) return node;
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    MaybeValue getMin() {
        NodeP node = findMinNode(root);
        if (node != nullptr) {
            return std::make_pair(node->value, true);
        }
        else {
            return std::make_pair(Value(), false);
        }
    }

    NodeP findMaxNode(NodeP node) {
        if (node == nullptr) return node;
        while (node->right != nullptr) {
            node = node->right;
        }
        return node;
    }

    MaybeValue getMax() {
        NodeP node = findMaxNode(root);
        if (node != nullptr) {
            return std::make_pair(node->value, true);
        }
        else {
            return std::make_pair(Value(), false);
        }
    }

    bool contains(Key key) {
        return get(key).second;
    }

    bool isEmpty() {
        return size() == 0;
    }

    size_t size() {
        return size(root);
    }

    size_t size(NodeP node) {
        if (node != nullptr) {
            return node->size();
        }
        return 0;
    }

    bool isBSTInefficient() {
        bool is = true;
        isBSTInefficient(root, is);
        return is;
    }

    void isBSTInefficient(NodeP node, bool& is) {
        if (node == nullptr) return;
        if (is == false) return;
        if (node->left != nullptr) {
            is &= findMaxNode(node->left)->key <= node->key;
            isBSTInefficient(node->left, is);
        }
        if (node->right != nullptr) {
            is &= findMinNode(node->right)->key >= node->key;
            isBSTInefficient(node->right, is);
        }
    }

    bool isBST() {
        bool is = true;
        if (size() < 2) return true;
        isBST(root, is, std::numeric_limits<Key>::min(), std::numeric_limits<Key>::max());
        return is;
    }

    void isBST(NodeP node, bool& is, Key min, Key max) {
        if (node == nullptr) return;
        if (is == false) return;

        is &= node->key >= min;
        is &= node->key <= max;

        if (node->left != nullptr) {
            isBST(node->left, is, min, node->key);
        }
        if (node->right != nullptr) {
            isBST(node->right, is, node->key, max);
        }
    }

    enum class Order {PRE_ORDER, IN_ORDER, POST_ORDER};

    template <typename Func>
    void traverse(Func f, Order order = Order::IN_ORDER) {
        traverse(root, f, order);
    };

    template <typename Func>
    void traverse(NodeP node, Func f, Order order = Order::IN_ORDER) {
        if (node == nullptr) return;

        if (order == Order::PRE_ORDER) {
            f(node);
        }

        traverse(node->left, f, order);

        if (order == Order::IN_ORDER) {
            f(node);
        }

        traverse(node->right, f, order);

        if (order == Order::POST_ORDER) {
            f(node);
        }
    };

    void print(Order order = Order::IN_ORDER) {
        traverse([](NodeP& node){
            if (node != nullptr) {
                std::cout << *node << " ";
            }
        }, order);
        std::cout << std::endl;
    }

private:
    NodeP root;
};

template <typename Key, typename Value>
void printMaybeValue(typename BST<Key, Value>::MaybeValue maybeValue) {
    if (maybeValue.second) {
        std::cout << maybeValue.first << std::endl;
    } else {
        std::cout << "Element not found.\n";
    }
}


void testBST() {
    BST<int, int> bst;
    bst.insert(2, 2);
    bst.insert(1, 1);
    bst.insert(5, 5);
    bst.insert(3, 3);
    bst.insert(4, 4);
    bst.print();
    auto elem = bst.get(4);
    printMaybeValue<int, int>(elem);
    bst.remove(2);
    bst.print();
    printMaybeValue<int, int>(bst.getMax());
    printMaybeValue<int, int>(bst.getMin());
    std::cout << "Tree is a BST: " << bst.isBST() << std::endl;
}


#endif //ALGS_BST_H
