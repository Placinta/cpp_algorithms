//
// Created by Placinta on 5/17/15.
//

#ifndef ALGS_HASH_TABLE_H
#define ALGS_HASH_TABLE_H

class Person {
public:
    Person() {}
    Person(std::string first, std::string last) : first_name(first), last_name(last) {}

    size_t hash() const {
        size_t hash_code = 17;
        std::hash<std::string> string_hash_fn;
        hash_code += 31 * hash_code + string_hash_fn(first_name);
        hash_code += 31 * hash_code + string_hash_fn(last_name);
        return hash_code;
    }

    bool operator==(const Person& other) {
        return first_name == other.first_name && last_name == other.last_name;
    }

    friend std::ostream& operator<<(std::ostream& os, const Person& person) {
        os << person.first_name << " " << person.last_name;
        return os;
    }

private:
    std::string first_name = "";
    std::string last_name = "";
};

namespace std {
    template<>
    struct hash<Person> {
        typedef Person argument_type;
        typedef size_t result_type;

        result_type operator()(argument_type const& person) {
            return person.hash();
        }
    };
}

typedef double Money;

template <typename Key, typename Value>
class ChainingHashSymbolTable {
    struct LinkedListNode;
    typedef std::shared_ptr<LinkedListNode> NodeP;
    typedef std::shared_ptr<NodeP> BucketP;

public:
    typedef std::pair<Key, bool> MaybeKey;
    typedef std::pair<Value, bool> MaybeValue;

private:
    typedef struct LinkedListNode {
        Key key;
        Value value;
        NodeP next;

        LinkedListNode() : key(), value(), next(nullptr) {}
        LinkedListNode(Key _key, Value _val) : key(key), value(_val), next(nullptr) {}
        LinkedListNode(Key _key, Value _val, NodeP _next_node) : key(_key), value(_val), next(_next_node) {}

        bool operator==(const LinkedListNode& other) {
            return key == other.key;
        }

        bool operator==(const Key& other_key) {
            return key == other_key;
        }

        bool operator!=(const LinkedListNode& other) {
            return key != other.key;
        }

        bool operator<(const LinkedListNode& other) {
            return key < other.key;
        }

        bool operator<=(const LinkedListNode& other) {
            return key <= other.key;
        }

        bool operator>(const LinkedListNode& other) {
            return key > other.key;
        }

        bool operator>=(const LinkedListNode& other) {
            return key >= other.key;
        }

        friend std::ostream& operator<<(std::ostream& os, const LinkedListNode& node) {
            os << "(" << node.key << ", " << node.value << ")";
            return os;
        }

        static void LinkedListNodeRemoveLog (LinkedListNode* removed_node) {
            std::cout << "Removing node " << *removed_node << std::endl;
            delete removed_node;
        }

    } LinkedListNode;

public:
    ChainingHashSymbolTable() : bucket_count(97), element_count(0), buckets(BucketP(new NodeP[bucket_count], std::default_delete<NodeP[]>())) {
    }

    ChainingHashSymbolTable(int _bucket_count) : bucket_count(_bucket_count), element_count(0), buckets(BucketP(new NodeP[bucket_count], std::default_delete<NodeP[]>())) {
    }

    MaybeValue get(Key key) {
        auto bucket = hashBucket(key);
        for (NodeP node = getBucketNode(bucket); node != nullptr; node = node->next) {
            if (node->key == key) {
                return std::make_pair(node->value, true);
            }
        }
        return std::make_pair(Value(), false);
    }

    void insert(Key key, Value value) {
        if (element_count >= bucket_count) resize(bucket_count * 2);
        auto bucket = hashBucket(key);
        for (NodeP node = getBucketNode(bucket); node != nullptr; node = node->next) {
            if (node->key == key) {
                node->value = value;
                return;
            }
        }
        getBucketNode(bucket) = NodeP(new LinkedListNode(key, value, getBucketNode(bucket)));
//        getBucketNode(bucket) = NodeP(new LinkedListNode(key, value, getBucketNode(bucket)), NodeRemoveLog);
        element_count++;
    }

    void remove(Key key) {
        if (!contains(key)) return;
        auto bucket = hashBucket(key);
        for (NodeP& node = getBucketNode(bucket), prev = nullptr; node != nullptr; prev = node, node = node->next) {
            if (node->key == key) {
                // Is middle or last element.
                if (prev != nullptr) {
                    prev->next = node->next;
                }
                else {
                    // Is first element.
                    node = nullptr;
                }
                break;
            }
        }
        element_count--;

        if (element_count > 0 && element_count <= bucket_count / 8) resize(bucket_count / 2);
    }

    bool contains(Key key) {
        return get(key).second;
    }

    size_t size() {
        return element_count;
    }

    bool isEmpty() {
        return size() == 0;
    }

    typedef std::pair<const Key, Value> value_type;

    class iterator : public std::iterator<std::forward_iterator_tag, value_type> {
        int bucket_index;
        ChainingHashSymbolTable& st;
        NodeP current_node;
    public:
        iterator(int _first_bucket_index, ChainingHashSymbolTable& _st, NodeP _first_node) :
                bucket_index(_first_bucket_index), st(_st), current_node(_first_node) { if (current_node == nullptr) increment(); }

        void increment() {
            while (bucket_index < st.bucket_count) {
                if (current_node != nullptr) {
                    current_node = current_node->next;
                }
                if (current_node == nullptr) {
                    ++bucket_index;
                    if (bucket_index < st.bucket_count) {
                        current_node = st.getBucketNode(bucket_index);
                        if (current_node != nullptr) break;
                    }
                    else break;
                }
                else break;
            }
        }

        iterator& operator++() {
            increment();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp(*this);
            increment();
            return tmp;
        }

        bool operator==(const iterator &other) const {
            return bucket_index == other.bucket_index && ((current_node == nullptr && other.current_node == nullptr) || (current_node == other.current_node));
        }

        bool operator!=(const iterator &other) const {
            return !((*this) == other);
        }

        value_type operator*() const {
            return std::make_pair(current_node->key, current_node->value);
        }
    };

    iterator begin() {
        if (isEmpty()) return end();
        return iterator(0, *this, getBucketNode(0));
    }
    iterator end() { return iterator(bucket_count, *this, nullptr); }

protected:
    void resize(int new_bucket_count) {
        ChainingHashSymbolTable new_st(new_bucket_count);
        for (int i = 0; i < bucket_count; i++) {
            for (NodeP node = getBucketNode(i); node != nullptr; node = node->next) {
                new_st.insert(node->key, node->value);
            }
        }
        std::swap(buckets, new_st.buckets);
        bucket_count = new_bucket_count;
    }

    int hashBucket(Key key) {
        return (int) ((std::hash<Key>()(key) & 0x07ffffffff) % bucket_count);
    }

    NodeP& getBucketNode(int index) { return buckets.get()[index]; }

    static void NodeRemoveLog (LinkedListNode* removed_node) {
        std::cout << "Removing node " << *removed_node << std::endl;
        delete removed_node;
    }

private:
    int bucket_count;
    size_t element_count;
    BucketP buckets;
};

template <typename Key, typename Value>
class LinearProbingHashSymbolTable {
    typedef std::shared_ptr<Key> KeyArrayP;
    typedef std::shared_ptr<Value> ValueArrayP;
    typedef std::shared_ptr<bool> BoolArrayP;

public:
    typedef std::pair<Key, bool> MaybeKey;
    typedef std::pair<Value, bool> MaybeValue;

public:
    LinearProbingHashSymbolTable() : LinearProbingHashSymbolTable(default_capacity) {}

    LinearProbingHashSymbolTable(size_t _capacity) : capacity(_capacity) {
        auto p = booleans.get();
        std::uninitialized_fill(p, p + capacity, false);
    }

    MaybeValue get(Key key) {
        auto i = hashCode(key);
        for (; getBool(i) != false; i = (i + 1) % capacity) {
            if (getKey(i) == key) {
                return std::make_pair(getValue(i), true);
            }
        }
        return std::make_pair(Value(), false);
    }

    void insert(Key key, Value value) {
        if (element_count >= capacity / 2) resize(capacity * 2);

        auto i = hashCode(key);
        for (; getBool(i) != false; i = (i + 1) % capacity) {
            if (getKey(i) == key) {
                getValue(i) = value;
                return;
            }
        }

        getKey(i) = key;
        getValue(i) = value;
        getBool(i) = true;

        element_count++;
    }

    void remove(Key key) {
        if (!contains(key)) return;

        auto i = hashCode(key);
        for (; getBool(i) != false; i = (i + 1) % capacity) {
            if (getKey(i) == key) {
                break;
            }
        }

        getBool(i) = false;
        i = (i + 1) % capacity;
        while (getBool(i) != false) {
            auto temp_key = getKey(i);
            auto temp_value = getValue(i);
            getBool(i) = false;
            element_count--;
            insert(temp_key, temp_value);
            i = (i + 1) % capacity;
        }

        element_count--;
        if (element_count > 0 && element_count <= capacity / 8) resize(capacity / 2);
    }

    bool contains(Key key) {
        return get(key).second;
    }

    size_t size() {
        return element_count;
    }

    bool isEmpty() {
        return size() == 0;
    }

    typedef std::pair<const Key, Value> value_type;

    class iterator : public std::iterator<std::forward_iterator_tag, value_type> {
        size_t index;
        LinearProbingHashSymbolTable& st;
    public:
        iterator(size_t _index, LinearProbingHashSymbolTable& _st) : index(_index), st(_st) {
            while (index < st.capacity && st.getBool(index) == false) {
                ++index;
            }
        }

        void increment() {
            do {
                ++index;
            }
            while (index < st.capacity && st.getBool(index) == false);
        }

        iterator& operator++() {
            increment();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp(*this);
            increment();
            return tmp;
        }

        bool operator==(const iterator &other) const {
            return index == other.index;
        }

        bool operator!=(const iterator &other) const {
            return !((*this) == other);
        }

        value_type operator*() const {
            return std::make_pair(st.getKey(index), st.getValue(index));
        }
    };

    iterator begin() {
        if (isEmpty()) return end();
        return iterator(0, *this);
    }
    iterator end() { return iterator(capacity, *this); }


protected:
    void resize(size_t new_capacity) {
        LinearProbingHashSymbolTable new_st(new_capacity);
        for (size_t i = 0; i < capacity; ++i) {
            if (getBool(i) == true) {
                new_st.insert(getKey(i), getValue(i));
            }
        }
        std::swap(keys, new_st.keys);
        std::swap(values, new_st.values);
        std::swap(booleans, new_st.booleans);
        capacity = new_capacity;
    }

    size_t hashCode(Key key) {
        return ((std::hash<Key>()(key) & 0x07ffffffff) % capacity);
    }

    Key& getKey(size_t index) { return keys.get()[index]; }
    Value& getValue(size_t index) { return values.get()[index]; }
    bool& getBool(size_t index) { return booleans.get()[index]; }

private:
    const static size_t default_capacity = 10;
    size_t capacity;
    size_t element_count = 0;
    KeyArrayP keys = KeyArrayP(new Key[capacity], std::default_delete<Key[]>());
    ValueArrayP values = ValueArrayP(new Value[capacity], std::default_delete<Value[]>());
    BoolArrayP booleans = BoolArrayP(new bool[capacity], std::default_delete<bool[]>());
};

template <typename Key, typename Value>
void printHashMaybeValue(typename ChainingHashSymbolTable<Key, Value>::MaybeValue maybeValue) {
    if (maybeValue.second) {
        std::cout << maybeValue.first << std::endl;
    } else {
        std::cout << "Element not found.\n";
    }
}

template <template <class, class> class HashTable>
void testHashTableImpl(std::string impl_name) {
    std::cout << "Test hash table - " << impl_name << ".\n";
    HashTable<Person, Money> chain_st;
    Person person1 = {"John", "Doe"};
    Person person2 = {"Genghis", "Kahn"};
    Person person3 = {"Walder", "Frey"};
    chain_st.insert(person1, 10);
    chain_st.insert(person2, 40000);
    chain_st.insert(person3, 30);

    auto maybe_money1 = chain_st.get(person1);
    auto maybe_money2 = chain_st.get(person2);
    auto maybe_money3 = chain_st.get(person3);
    std::cout << person1 << " " ; printHashMaybeValue<Person, Money>(maybe_money1);
    std::cout << person2 << " " ; printHashMaybeValue<Person, Money>(maybe_money2);
    std::cout << person3 << " " ; printHashMaybeValue<Person, Money>(maybe_money3);

    chain_st.remove(person2);

    std::cout << "Iterator test.\n";
    for (auto it = chain_st.begin(); it != chain_st.end(); it++) {
        std::cout << (*it).first << " " << (*it).second << std::endl;
    }


    std::cout << "Resizing test.\n";
    std::cout << "Insert 100 ints.\n";
    HashTable<int, int> chain_st2(2);
    for (int i = 0; i < 100; i++) {
        chain_st2.insert(i, i);
    }
    for (auto it = chain_st2.begin(), end = chain_st2.end(); it != end; it++) {
        std::cout << (*it).first << " " << (*it).second << std::endl;
    }

    std::cout << "Remove 95 ints.\n";
    for (int i = 0; i < 95; i++) {
        chain_st2.remove(i);
    }

    for (auto it = chain_st2.begin(), end = chain_st2.end(); it != end; it++) {
        std::cout << (*it).first << " " << (*it).second << std::endl;
    }

};

void testHashTable() {
    testHashTableImpl<ChainingHashSymbolTable>("separate chaining");
    testHashTableImpl<LinearProbingHashSymbolTable>("linear probing");
}

#endif //ALGS_HASH_TABLE_H
