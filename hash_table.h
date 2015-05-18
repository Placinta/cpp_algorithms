//
// Created by Placinta on 5/17/15.
//

#ifndef ALGS_HASH_TABLE_H
#define ALGS_HASH_TABLE_H

class Person {
public:
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
    std::string first_name;
    std::string last_name;
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
//        getBucketNode(bucket) = NodeP(new LinkedListNode(key, value, getBucketNode(bucket)));
        getBucketNode(bucket) = NodeP(new LinkedListNode(key, value, getBucketNode(bucket)), NodeRemoveLog);
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
void printHashMaybeValue(typename ChainingHashSymbolTable<Key, Value>::MaybeValue maybeValue) {
    if (maybeValue.second) {
        std::cout << maybeValue.first << std::endl;
    } else {
        std::cout << "Element not found.\n";
    }
}

void testHashTable() {
    std::cout << "Test hash table - separate chaining.\n";
    ChainingHashSymbolTable<Person, Money> chain_st;
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
    ChainingHashSymbolTable<int, int> chain_st2(2);
    for (int i = 0; i < 100; i++) {
        chain_st2.insert(i, i);
    }
    for (auto it = chain_st2.begin(), end = chain_st2.end(); it != end; it++) {
        std::cout << (*it).first << " " << (*it).second << std::endl;
    }

    std::cout << "Remove 90 ints.\n";
    for (int i = 0; i < 95; i++) {
        chain_st2.remove(i);
    }

    for (auto it = chain_st2.begin(), end = chain_st2.end(); it != end; it++) {
        std::cout << (*it).first << " " << (*it).second << std::endl;
    }

};

#endif //ALGS_HASH_TABLE_H
