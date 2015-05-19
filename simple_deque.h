//
// Created by Placinta on 5/19/15.
//

#ifndef ALGS_SIMPLE_DEQUE_H
#define ALGS_SIMPLE_DEQUE_H

#include <memory>
#include <iostream>

template <typename T>
class Deque {
private:

    class DNode;
    typedef std::shared_ptr<DNode> DNodeP;
    typedef std::weak_ptr<DNode> DNodeWP;
    typedef std::pair<T, bool> MaybeValue;

    class DNode {
    public:
        DNode() : value() {}
        DNode(T _val) : value(_val) {}
        DNode(T _val, DNodeP _next, DNodeWP _prev = DNodeWP()) : value(_val), next(_next), prev(_prev) {}
        T getValue() const { return value; }
        DNodeP getNext() { return next; }
        DNodeWP getPrev() { return prev; }
        void setPrev(DNodeP node) { prev = node; }
        void setNext(DNodeP node) { next = node; }

        friend std::ostream& operator<<(std::ostream& os, const DNode& node) {
            os << node.getValue();
            return os;
        }
    private:
        T value;
        DNodeP next = nullptr;
        DNodeWP prev = DNodeWP();
    };

public:
    Deque() {}
    bool isEmpty() { return size() == 0; }
    size_t size() { return element_count; }
    void addFirst(T val) {
        auto node = DNodeP(new DNode(val, first));
        if (first != nullptr) first->setPrev(node);
        first = node;
        if (last.expired()) last = first;
        element_count++;

    }
    void addLast(T val) {
        auto node = DNodeP(new DNode(val, nullptr, last));
        if (!last.expired()) last.lock()->setNext(node);
        last = node;
        if (first == nullptr) first = node;
        element_count++;

    }
    MaybeValue removeFirst() {
        if (first == nullptr) return std::make_pair(T(), false);
        auto node = first;
        first = first->getNext();
        if (first == nullptr) last = DNodeWP();
        else first->setPrev(nullptr);
        element_count--;
        return std::make_pair(node->getValue(), true);
    }
    MaybeValue removeLast() {
        if (last.expired()) return std::make_pair(T(), false);
        auto node = last.lock();
        last = node->getPrev();
        if (last.expired()) first = nullptr;
        else last.lock()->setNext(nullptr);
        element_count--;
        return std::make_pair(node->getValue(), true);
    }


    typedef T value_type;
    class iterator : public std::iterator<std::bidirectional_iterator_tag, value_type> {
        DNodeP current_node;
    public:
        iterator(DNodeP _first_node) : current_node(_first_node) {}

        void increment() {
            current_node = current_node->getNext();
        }

        void decrement() {
            auto prev = current_node->getPrev();
            if (prev.expired()) current_node = nullptr;
            else current_node = prev.lock();
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

        iterator& operator--() {
            decrement();
            return *this;
        }

        iterator operator--(int) {
            iterator tmp(*this);
            decrement();
            return tmp;
        }

        bool operator==(const iterator &other) const {
            return current_node == other.current_node;
        }

        bool operator!=(const iterator &other) const {
            return !((*this) == other);
        }

        value_type operator*() const {
            return current_node->getValue();
        }
    };

    iterator begin() { return iterator(first); }
    iterator end() { return iterator(nullptr); }
    iterator rbegin() { if (last.expired()) return iterator(nullptr); else return iterator(last.lock()); }
    iterator rend() { return iterator(nullptr); }

protected:
    static void DNodeRemoveLog (DNode* removed_node) {
        std::cout << "Removing node " << *removed_node << std::endl;
        delete removed_node;
    }

private:
    DNodeP first = nullptr;
    DNodeWP last = DNodeWP();
    size_t element_count = 0;
};

void testDeque() {
    std::cout << "Test simple deque.\n";
    Deque<int> d;

    d.addFirst(1);
    d.addFirst(2);
    d.addLast(3);
    d.addLast(4);

    while (!d.isEmpty()) {
        auto pair = d.removeFirst();
        if (pair.second) std::cout << pair.first << "\n";
    }

    d.addFirst(5);
    d.addFirst(6);
    d.addLast(7);
    d.addLast(8);

    while (!d.isEmpty()) {
        auto pair = d.removeLast();
        if (pair.second) std::cout << pair.first << "\n";
    }

    for (int i = 0; i < 10; i++) d.addFirst(i);
    for (auto it = d.begin(), end = d.end(); it != end; it++) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    for (auto it = d.rbegin(), end = d.rend(); it != end; it--) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

#endif //ALGS_SIMPLE_DEQUE_H
