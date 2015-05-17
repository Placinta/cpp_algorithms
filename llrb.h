//
// Created by Placinta on 5/11/15.
//

#ifndef ALGS_LLRB_H
#define ALGS_LLRB_H

#include <iomanip>

template <typename Key, typename Value>
class LLRB {
public:
    struct TreeNode;
    typedef std::shared_ptr<TreeNode> NodeP;
    typedef std::pair<Key, bool> MaybeKey;
    typedef std::pair<Value, bool> MaybeValue;
    enum class Color {RED, BLACK};

    typedef struct TreeNode {
        TreeNode() : key(), value(), left(nullptr), right(nullptr), _size(0), color(Color::RED) {}
        TreeNode(Key _key, Value _val) : key(_key), value(_val), left(nullptr), right(nullptr), _size(1), color(Color::RED) {}
        TreeNode(Key _key, Value _val, NodeP _left, NodeP _right) : key(_key), value(_val), left(_left), right(_right), _size(0), color(Color::RED) {}

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
            return key >= other.key;
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
        Color color;

    private:
        size_t _size;
    } TreeNode;

    LLRB() : root() {}

    void insert(Key key, Value val) {
        root = insert(root, key, val);
        root->color = Color::BLACK;
        assert(checkIntegrity());
    }

    bool isRed(NodeP node) {
        return node != nullptr && node->color == Color::RED;
    }

    NodeP rotateLeft(NodeP node) {
        assert(isRed(node->right));
        NodeP t = node->right;
        auto node_2 = node;
        auto t_2 = t;
        node->right = t->left;
        t->left = node;
        t->color = node->color;
        node->color = Color::RED;
        node->size(1 + size(node->left) + size(node->right));
        return t;
    }

    NodeP rotateRight(NodeP node) {
        assert(isRed(node->left));
        NodeP t = node->left;
        auto node_2 = node;
        auto t_2 = t;
        node->left = t->right;
        t->right = node;
        t->color = node->color;
        node->color = Color::RED;
        node->size(1 + size(node->left) + size(node->right));
        return t;

    }

    void flipColors(NodeP node) {
        assert(node != nullptr && node->left != nullptr && node->right != nullptr);
        assert((!isRed(node) && isRed(node->left) && isRed(node->right)) || (isRed(node) && !isRed(node->left) && !isRed(node->right)));
        node->color = flipOneColor(node->color);
        node->left->color = flipOneColor(node->left->color);
        node->right->color = flipOneColor(node->right->color);
    }

    Color flipOneColor(Color color) {
        if (color == Color::RED) {
            return Color::BLACK;
        }
        return Color::RED;
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

        if (isRed(node->right) && !isRed(node->left)) { node = rotateLeft(node); }
        if (isRed(node->left) && isRed(node->left->left)) { node = rotateRight(node); }
        if (isRed(node->left) && isRed(node->right)) { flipColors(node); }

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
        if (!contains(key)) return;

        if (!isRed(root->left) && !isRed(root->right)) {
            root->color = Color::RED;
        }
        root = remove(root, key);
        if (!isEmpty()) {
            root->color = Color::BLACK;
        }
        assert(checkIntegrity());
    }

    NodeP remove(NodeP node, Key key) {
        if (node == nullptr) return nullptr;
        if (key < node->key) {
            if (!isRed(node->left) && !isRed(node->left->left)) {
                node = moveRedLeft(node);
            }
            node->left = remove(node->left, key);
        }
        else {
            if (isRed(node->left)) {
                node = rotateRight(node);
            }
            if (key == node->key && node->right == nullptr) {
                return nullptr;
            }
            if (!isRed(node->right) && !isRed(node->right->left)) {
                node = moveRedRight(node);
            }
            if (key == node->key) {
                auto min = findMinNode(node->right);
                node->value = min->value;
                node->key = min->key;
                node->right = deleteMin(node->right);
            }
            else {
                node->right = remove(node->right, key);
            }
        }

        node->size(1 + size(node->left) + size(node->right));
        return balance(node);
    }

    NodeP moveRedLeft(NodeP node) {
        assert(isRed(node));
        assert(!isRed(node->left));
        assert(!isRed(node->left->left));
        flipColors(node);
        if (isRed(node->right->left)) {
            node->right = rotateRight(node->right);
            node = rotateLeft(node);
            flipColors(node);
        }
        return node;
    }

    NodeP moveRedRight(NodeP node) {
        assert(isRed(node));
        assert(!isRed(node->right));
        assert(!isRed(node->right->left));
        flipColors(node);
        if (isRed(node->left->left)) {
            node = rotateRight(node);
            flipColors(node);
        }
        return node;
    }

    void deleteMin() {
        if (isEmpty()) {
            std::cerr << "No element to remove.\n";
            return;
        }

        if (!isRed(root->left) && !isRed(root->right)) {
            root->color = Color::RED;
        }
        root = deleteMin(root);
        if (!isEmpty()) root->color = Color::BLACK;
    }

    NodeP deleteMin(NodeP node) {
        if (node->left == nullptr) {
            return nullptr;
        }

        if (!isRed(node->left) && !isRed(node->left->left)) {
            node = moveRedLeft(node);
        }

        node->left = deleteMin(node->left);
        return balance(node);
    }

    NodeP balance(NodeP node) {
        assert(node != nullptr);
        if (isRed(node->right)) {node = rotateLeft(node); }
        if (isRed(node->left) && isRed(node->left->left)) { node = rotateRight(node); }
        if (isRed(node->left) && isRed(node->right)) { flipColors(node); }

        node->size(1 + size(node->left) + size(node->right));

        return node;
    }

    size_t height() {
        return height(root);
    }

    size_t height(NodeP node) {
        if (node == nullptr) return 0;
        return 1 + std::max(height(node->left), height(node->right));
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

    /**
     * The k'th key in the tree (the key of rank k).
     */
    MaybeKey select(size_t k) {
        if (k < 0 || k >= size()) {
            return std::make_pair(Key(), false);
        }
        NodeP node = select(root, k);
        if (node != nullptr) {
            return std::make_pair(node->key, true);
        }
        return std::make_pair(Key(), false);
    }

    NodeP select(NodeP node, size_t k) {
        assert(node != nullptr);
        assert(k >= 0 && k < size(node));
        size_t t = size(node->left);
        if (k < t) return select(node->left, k);
        else if (k > t) return select(node->right, k - t - 1);
        else if (t == k) return node;
        else return nullptr;
    }

    /**
     * Number of keys less than k.
     */
    size_t rank(Key k) {
        return rank(root, k);
    }

    size_t rank(NodeP node, size_t k) {
        if (node == nullptr) return 0;
        if (k < node->key) {
            return rank(node->left, k);
        }
        else if (k > node->key) {
            return 1 + size(node->left) + rank(node->right, k);
        }
        else /* if (node->key == k) */ {
            return size(node->left);
        }
    }

    /**
     * The largest key less than or equal to k.
     */
    MaybeKey floor(Key k) {
        NodeP node = floor(root, k);
        if (node != nullptr) {
            return std::make_pair(node->key, true);
        }
        return std::make_pair(Key(), false);

    }

    NodeP floor(NodeP node, Key k) {
        if (node == nullptr) return nullptr;
        if (k < node->key) {
            return floor(node->left, k);
        }
        else if (k == node->key) {
            return node;
        }
        NodeP right = floor(node->right, k);
        if (right != nullptr) return right;
        else return node;
    }

    /**
     * The smallest key greater than or equal to k.
     */
    MaybeKey ceiling(Key k) {
        NodeP node = ceiling(root, k);
        if (node != nullptr) {
            return std::make_pair(node->key, true);
        }
        return std::make_pair(Key(), false);
    }

    NodeP ceiling(NodeP node, Key k) {
        if (node == nullptr) return nullptr;
        if (k > node->key) {
            return ceiling(node->right, k);
        }
        else if (k == node->key) {
            return node;
        }
        NodeP left = ceiling(node->left, k);
        if (left != nullptr) return left;
        else return node;
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

    bool isBSTInorderTraversal() {
        if (size() < 2) return true;
        NodeP prev = nullptr;
        return isBSTInorderTraversal(root, prev);
    }

    bool isBSTInorderTraversal(NodeP node, NodeP& prev) {
        if (node == nullptr) return true;
        if (!isBSTInorderTraversal(node->left, prev)) {
            return false;
        }

        if (prev != nullptr && node->key <= prev->key) {
            return false;
        }

        prev = node;
        return isBSTInorderTraversal(node->right, prev);
    }

    bool is23() {
        return is23(root);
    }

    bool is23(NodeP node) {
        if (node == nullptr) return true;
        if (isRed(node->right)) return false;
        if (isRed(node->left) && isRed(node)) return false;
        return is23(node->left) && is23(node->right);
    }

    bool isBalanced() {
        size_t max_black_links = 0;
        NodeP max = root;
        while (max != nullptr) {
            if (!isRed(max)) max_black_links++;
            max = max->left;
        }
        return isBalanced(root, max_black_links);
    }

    bool isBalanced(NodeP node, size_t current_black_links) {
        if (node == nullptr) return current_black_links == 0;
        if (!isRed(node)) current_black_links--;
        return isBalanced(node->left, current_black_links) && isBalanced(node->right, current_black_links);
    }

    bool isSizeConsistent() {
        return isSizeConsistent(root);
    }

    bool isSizeConsistent(NodeP node) {
        if (node == nullptr) return true;
        if (size(node) != size(node->left) + size(node->right) + 1) return false;
        return isSizeConsistent(node->left) && isSizeConsistent(node->right);
    }

    bool checkIntegrity() {
        if (!isBST()) std::cout << "Tree is not in symmetric order.\n";
        if (!isSizeConsistent()) std::cout << "Tree sizes are not correct.\n";
        if (!is23()) std::cout << "Tree is not in a 2-3 tree.\n";
        if (!isBalanced()) std::cout << "Tree is not balanced.\n";
        return isBST() && isSizeConsistent() && is23() && isBalanced();
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

    typedef std::pair<const Key, Value> value_type;

    class iterator : public std::iterator<std::forward_iterator_tag, value_type> {
        NodeP root, right, curr, parent, item, pred;
        Order order;
    public:
        iterator(NodeP _root, Order _order = Order::IN_ORDER) : root(_root), right(nullptr), curr(_root), parent(_root),
                                item(nullptr), order(_order) { increment(); }

        void increment() {
            if (order == Order::PRE_ORDER) incrementPreOrder();
            if (order == Order::IN_ORDER) incrementInOrder();
        }

        void incrementPreOrder() {
            bool element_found = false;
            item = nullptr;
            while (parent != nullptr && !element_found) {
                curr = parent->left;
                if (curr != nullptr) {
                    // search for thread
                    while (curr != right && curr->right != nullptr)
                        curr = curr->right;

                    if (curr != right) {
                        // insert thread
                        assert(curr->right == nullptr);
                        curr->right = parent;
                        // The item to be traversed.
                        item = parent;
                        parent = parent->left;
                        element_found = true;
                        continue;
                    } else
                        // remove thread, left subtree of P already traversed
                        // this restores the node to original state
                        curr->right = nullptr;
                } else {
                    // The item to be traversed.
                    item = parent;
                    element_found = true;
                }

                right = parent;
                parent = parent->right;
            }
        }

        void incrementInOrder() {
            bool element_found = false;
            item = nullptr;
            while (curr != nullptr && !element_found) {
                auto curr_item = *curr;
                if (curr->left == nullptr) {
                    item = curr;
                    curr = curr->right;
                    element_found = true;
                }
                else {
                    // Find predecessor.
                    pred = curr->left;
                    while (pred->right != nullptr && pred->right != curr) {
                        pred = pred->right;
                    }

                    if (pred->right == nullptr) {
                        // Make current be the succesor of predecesor (link back to parent essentially).
                        pred->right = curr;
                        curr = curr->left;
                    }
                    else {
                        // Remove the link back, because we already traversed the whole left sub-tree.
                        pred->right = nullptr;
                        item = curr;
                        element_found = true;
                        curr = curr->right;
                    }
                }
            }
        }

        iterator &operator++() {
            increment();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp(*this);
            increment();
            return tmp;
        }

        bool operator==(const iterator &other) const {
            return (item == nullptr && other.item == nullptr) || (item != nullptr && other.item != nullptr && *item == *other.item);
        }

        bool operator!=(const iterator &other) const {
            return !((*this) == other);
        }

        value_type operator*() const {
            return std::make_pair(item->key, item->value);
        }
    };

    iterator begin() { return iterator(root); }
    iterator beginPreOrder() { return iterator(root, Order::PRE_ORDER); }
    iterator end() { return iterator(nullptr); }

    void printPreOrderValues() {
        std::cout << "Pre order iterator traversal.\n";
        for (auto it = beginPreOrder(); it != end(); it++) {
            std::cout << (*it).second << " ";
        }
        std::cout << std::endl;
    }

    void printInOrderValues() {
        std::cout << "In order iterator traversal.\n";
        for (auto pair: *this) {
            std::cout << pair.second << " ";
        }
        std::cout << std::endl;
    }

    void printASCIITree(bool use_max_node_key_length_as_max_width = true, size_t max_key_length = 5, size_t max_value_length = 5) {
        if (isEmpty()) return;

        // Prepare an array of strings that will be manipulated to display the tree nodes.
        size_t additional_root_color_row = 1;
        std::vector<std::string> output(height() + additional_root_color_row);

        size_t key_length;
        size_t value_length;

        // Find the longest key, to define the width. This is not 100% full-proof.
        if (use_max_node_key_length_as_max_width) {
            NodeP max = findMaxNode(root);
            Key key = max->key;
            Value val = max->value;
            key_length = std::to_string(key).length();
            value_length = std::to_string(val).length();
        }
        else {
            key_length = max_key_length;
            value_length = max_value_length;
        }

        // "(Key, Value)" character length
        size_t width = key_length + value_length + 4;

        printASCIITree(root, 0, 0, 1, output, width, key_length, value_length);

        for(auto& row : output) {
            std::cout << row << std::endl;
        }
    }

    void stringResizeIfSmaller(std::string& s, size_t capacity, char fillChar = ' ') {
        if (s.length() <  capacity) {
            s.resize(capacity, fillChar);
        }
    }

    char getASCIINodeColor(NodeP node) {
        if (isRed(node)) return 'R';
        else return 'B';
    }

    size_t printASCIITree(NodeP node, bool is_left, size_t offset, size_t depth, std::vector<std::string>& output, const size_t width, const size_t key_length, const size_t value_length) {
        if (node == nullptr) return 0;

        // Create the key, value pair string output representation.
        std::stringstream ss;
        ss << '(' << std::setw(
                static_cast<int>(key_length)) << node->key << ", " << std::setw(
                static_cast<int>(value_length)) << node->value << ')';
        std::string pair_output(ss.str());

        // Get how many characters does the left and right sub-trees occupy, as well as print them into the buffer.
        size_t left = printASCIITree(node->left, true, offset, depth + 1, output, width, key_length, value_length);
        size_t right = printASCIITree(node->right, false, offset + left + width, depth + 1, output, width, key_length, value_length);

        // Compute how many characters will be needed to store the 'depth'-th row of output.
        size_t char_length_required = offset + left + width;
        stringResizeIfSmaller(output[depth], char_length_required, ' ');

        // Add the output of the key-value pair to the output buffer.
        output[depth].replace(offset + left, pair_output.length(), pair_output);

        // Draw the left arrows and node color.
        if (depth && is_left) {
            size_t replace_count = left + width / 2;
            stringResizeIfSmaller(output[depth - 1], offset + left + width / 2 + replace_count, ' ');
            output[depth - 1].replace(offset + left + width / 2, replace_count, replace_count, '-');
            output[depth - 1].replace(offset + left + width / 2, 1, 1, getASCIINodeColor(node));
        }

        // Draw right arrows and node color.
        else if (depth && !is_left) {
            size_t replace_count = left + width / 2;
            stringResizeIfSmaller(output[depth - 1], offset + replace_count, ' ');
            output[depth - 1].replace(offset, replace_count, replace_count, '-');
            output[depth - 1].replace(offset + replace_count, 1, 1, getASCIINodeColor(node));
        }

        // Root node.
        if (depth == 1) {
            // Make sure no lines are displayed for root node.
            output[depth - 1].replace(0, left + width / 2, left + width / 2, ' ');

            // Display the root color.
            output[depth - 1].replace(left + width / 2, 1, 1, getASCIINodeColor(node));
        }


        // Return the length of characters that the sub-tree uses to display itself.
        // This includes the number of characters for the left sub tree + current sub-tree root + right sub tree.
        return left + width + right;
    }

private:
    NodeP root;
};

template <typename Key, typename Value>
void printMaybeKey(typename LLRB<Key, Value>::MaybeKey maybeKey) {
    if (maybeKey.first) {
        std::cout << maybeKey.first << std::endl;
    } else {
        std::cout << "Key not found.\n";
    }
}

void testLLRB() {
    LLRB<int, int> llrb;
    llrb.insert(5, 5);
    llrb.insert(4, 4);
    llrb.insert(3, 3);
    llrb.insert(2, 2);
    llrb.insert(1, 1);
    llrb.insert(6, 6);
    llrb.insert(7, 7);
    llrb.insert(8, 8);
    llrb.insert(9, 9);
    llrb.insert(10, 10);
    llrb.insert(11, 11);
    llrb.insert(12, 12);
    llrb.insert(13, 13);
    llrb.insert(14, 14);
    llrb.insert(15, 15);
    llrb.insert(16, 16);
    llrb.print();
    auto elem = llrb.get(4);
    printMaybeValue<int, int>(elem);
    llrb.print();
    std::cout << "Height of tree is: " << llrb.height() << std::endl;
    std::cout << "Max: "; printMaybeValue<int, int>(llrb.getMax());
    std::cout << "Min: "; printMaybeValue<int, int>(llrb.getMin());
    size_t rank = 2;
    std::cout << "Element of rank " << rank << ": "; printMaybeKey<int, int>(llrb.select(rank));;
    int key = 3;
    std::cout << "Rank of key " << key << ": " << llrb.rank(key) << std::endl;
    int floor = 7;
    int ceiling = 0;
    std::cout << "Floor of key " << floor << ": "; printMaybeKey<int, int>(llrb.floor(floor));
    std::cout << "Ceiling of key " << ceiling << ": "; printMaybeKey<int, int>(llrb.ceiling(ceiling));
    std::cout << "Tree is a BST: " << llrb.isBST() << std::endl;
    std::cout << "Tree is a BST: " << llrb.isBSTInefficient() << std::endl;
    std::cout << "Tree is a BST: " << llrb.isBSTInorderTraversal() << std::endl;
    std::cout << "Tree sizes are correct: " << llrb.isSizeConsistent() << std::endl;
    std::cout << "Is 2-3 tree: " << llrb.is23() << std::endl;
    std::cout << "Is balanced tree: " << llrb.isBalanced() << std::endl;
    llrb.checkIntegrity();

    llrb.printPreOrderValues();
    llrb.printInOrderValues();

    llrb.printASCIITree(false, 1, 1);

    // Remove some items.
    llrb.remove(2);
    llrb.remove(10);
    llrb.remove(4);
    llrb.remove(11);
    llrb.remove(16);
    llrb.remove(8);
    llrb.remove(1);
    llrb.remove(12);
    llrb.remove(5);
    llrb.remove(3);

    llrb.printASCIITree(false, 1, 1);
}


#endif //ALGS_LLRB_H
