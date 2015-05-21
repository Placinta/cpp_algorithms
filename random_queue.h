//
// Created by Placinta on 5/21/15.
//

#ifndef ALGS_RANDOM_QUEUE_H
#define ALGS_RANDOM_QUEUE_H

#include <memory>
#include <iostream>
#include <random>
#include "sorts.h"

template <typename T>
class RandomQueue {
    using TP = std::shared_ptr<T>;
public:
    RandomQueue() : elements(new T[10], std::default_delete<T[]>()) {}
    void enqueue(T item) {
        if (element_count == capacity) resize(capacity * 2);
        get(last) = item;
        ++element_count;
        size_t index = (first + getRandomIndex()) % capacity;
        std::swap(get(index), get(last));
        last = (last + 1) % capacity;
    }
    T dequeue() {
        auto item = get(first);
        first = (first + 1) % capacity;
        --element_count;
        if (element_count > 0 && element_count == capacity / 4) resize(capacity / 2);
        return item;
    }
    T sample() {
        return get(first);
    }
    size_t size() { return element_count; }
    bool isEmpty() { return size() == 0; }

    typedef T value_type;
    class iterator : public std::iterator<std::forward_iterator_tag, value_type> {
    public:
        iterator() = delete;

        iterator(TP items, size_t _element_count, size_t capacity, size_t first) : element_count(_element_count) {
            if (element_count == 0 || items == nullptr) {
                index = element_count;
                return;
            }
            elements = TP(new T[element_count], std::default_delete<T[]>());
            for (size_t i = 0; i < element_count; ++i, first = (first + 1) % capacity) {
                elements.get()[i] = items.get()[first];
            }
            knuth_shuffle(elements.get(), elements.get() + element_count);
        }

        iterator(const iterator& other) : elements(nullptr), index(other.index), element_count(other.element_count) {
            if (element_count == 0) return;
            elements = TP(new T[other.element_count], std::default_delete<T[]>());
            for (size_t i = 0; i < element_count; ++i) {
                elements.get()[i] = other.elements.get()[i];
            }
        }

        iterator& operator=(const iterator& other) {
            iterator tmp(other);
            std::swap(elements, other.elements);
            std::swap(index, other.index);
            std::swap(element_count, other.element_count);
            return *this;
        }

        bool operator==(const iterator& other) const {
            return index == other.index;
        }

        bool operator!=(const iterator& other) const {
            return index != other.index;
        }

        void increment() {
            ++index;
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

        value_type operator*() const {
            return elements.get()[index];
        }

    private:
        TP elements = nullptr;
        size_t index = 0;
        size_t element_count = 0;
    };

    iterator begin() { return iterator(elements, element_count, capacity, first); }
    iterator end() { return iterator(nullptr, element_count, element_count, element_count); }

protected:
    void resize(size_t new_capacity) {
        TP new_elements = TP(new T[new_capacity], std::default_delete<T[]>());
        for (size_t i = 0; i < element_count; ++i, first = (first + 1) % capacity)
            new_elements.get()[i] = get(first);
        std::swap(elements, new_elements);
        capacity = new_capacity;
        first = 0;
        last = element_count;
    }

    T& get(size_t index) {
        return elements.get()[index];
    }

    size_t getRandomIndex() {
        std::random_device rd;
        std::mt19937 gen(rd());
        return std::uniform_int_distribution<unsigned long>{0, element_count - 1}(gen);
    }
private:
    TP elements = nullptr;
    size_t element_count = 0;
    size_t capacity = 10;
    size_t first = 0;
    size_t last = 0;
};

void testRandomQueue() {
    std::cout << "Test randomized queue.\n";
    RandomQueue<int> rq;
    std::cout << "Enqueue / dequeue test.\n";
    rq.enqueue(1);
    rq.enqueue(2);
    rq.enqueue(3);

    std::cout << rq.sample();
    std::cout << "\n";

    while (!rq.isEmpty()) {
        std::cout << rq.dequeue() << " ";
    }
    std::cout << "\n";

    std::cout << "Two mutually exclusive random iterator tests.\n";
    for (int i = 0; i < 10; i++) {
        rq.enqueue(i);
    }
    auto it1 = rq.begin();
    for (auto it1_end = rq.end(); it1 != it1_end; ++it1) {
        for (auto it2 = rq.begin(), it2_end = rq.end(); it2 != it2_end; ++it2) {
            std::cout << "(" << *it1 << ", " << *it2 << ")";
        }
        std::cout << std::endl;
    }
    std::cout << "\n";

    std::cout << "Second enqueue / dequeue test.\n";
    while (!rq.isEmpty()) {
        std::cout << rq.dequeue() << " ";
    }
    std::cout << "\n";

}

#endif //ALGS_RANDOM_QUEUE_H
