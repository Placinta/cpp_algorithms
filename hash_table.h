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
        auto bucket = hashBucket(key);
        for (NodeP node = getBucketNode(bucket); node != nullptr; node = node->next) {
            if (node->key == key) {
                node->value = value;
                return;
            }
        }
        getBucketNode(bucket) = NodeP(new LinkedListNode(key, value, getBucketNode(bucket)));
        element_count++;
    }

    size_t size() {
        return element_count;
    }

    bool isEmpty() {
        return size() != 0;
    }

protected:
    void resize(int new_bucket_count) {
        BucketP new_buckets(new NodeP[new_bucket_count], std::default_delete<NodeP[]>());
        for (long i = 0; i < bucket_count; i++) {
            new_buckets.get()[i] = buckets.get()[i];
        }
        std::swap(buckets, new_buckets);
        bucket_count = new_bucket_count;
    }

    int hashBucket(Key key) {
        return (int) ((std::hash<Key>()(key) & 0x07ffffffff) % bucket_count);
    }

    NodeP& getBucketNode(int index) { return buckets.get()[index]; }

private:
    int bucket_count;
    long element_count;
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
};

#endif //ALGS_HASH_TABLE_H
