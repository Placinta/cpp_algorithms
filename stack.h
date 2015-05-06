//
// Created by Placinta on 5/5/15.
//

#ifndef ALGS_STACK_H
#define ALGS_STACK_H

#include <memory>
#include <iostream>
#include <algorithm>
#include <iterator>
#include "linkedlistnode.h"

template <typename T>
class IStack {
public:
    virtual void push(T item) = 0;
    virtual T pop() = 0;
    virtual bool isEmpty() = 0;
    virtual int size() = 0;
};

template <typename T>
class LinkedListStack : public IStack<T> {
public:
    LinkedListStack() : first(nullptr), element_count(0) {}

    void push(T item) {
        std::shared_ptr<LinkedListNode<T> > node = std::make_shared<LinkedListNode<T> >(item);
        node->next = first;
        first = node;
        element_count++;
    }

    T pop() {
        auto current_node = first;
        first = first->next;
        element_count--;
        return current_node->item;
    }

    bool isEmpty() {
        return first == nullptr;
    }

    int size() {
        return element_count;
    }

private:
    std::shared_ptr<LinkedListNode<T> > first;
    int element_count;
};

template <typename T>
class FixedCapacityStack : public IStack<T> {
public:
    FixedCapacityStack(int capacity) : elements(new T[capacity]), element_count(0) {}

    void push(T item) {
        elements[element_count++] = item;
    }

    T pop() {
        return elements[--element_count];
    }

    bool isEmpty() {
        return size() == 0;
    }

    int size() {
        return element_count;
    }

private:
    std::unique_ptr<T[]> elements;
    int element_count;
};

template <typename T>
class ResizingArrayStack : public IStack<T> {
public:
    ResizingArrayStack() : elements(new T[1]), element_count(0), capacity(1) {}

    void push(T item) {
        if (element_count == capacity) {
            resize(capacity * 2);
        }
        elements[element_count++] = item;
    }

    T pop() {
        if (element_count > 0 && element_count == capacity / 4) {
            resize(capacity / 2);
        }
        return elements[--element_count];
    }

    bool isEmpty() {
        return size() == 0;
    }

    int size() {
        return element_count;
    }

    void resize(int new_size) {
        std::unique_ptr<T[]> new_elements(new T[new_size]);
        std::copy_n(elements.get(), element_count, new_elements.get());
        elements.swap(new_elements);
        capacity = new_size;
    }

private:
    std::unique_ptr<T[]> elements;
    int element_count;
    int capacity;
};

template <typename T>
void testStackImpl(std::shared_ptr<IStack<T> > stack) {
    stack->push(1);
    stack->push(3);
    stack->push(2);

    std::cout << "Size: " << stack->size() << std::endl;

    while (!stack->isEmpty()) {
        std::cout << stack->pop() << std::endl;
    }

    std::cout << "Size: " << stack->size() << std::endl;

    stack->push(1);
    stack->push(3);
    stack->push(2);

    while (!stack->isEmpty()) {
        std::cout << stack->pop() << std::endl;
    }
}

void testStack() {
    std::cout << "Testing stack implementations.\n";
    std::cout << "Testing linked list based stack.\n";
    auto stack = std::make_shared<LinkedListStack<int> >();
    testStackImpl<int>(stack);

    std::cout << "Testing fixed capacity array based stack.\n";
    auto stack2 = std::make_shared<FixedCapacityStack<int> >(10);
    testStackImpl<int>(stack2);

    std::cout << "Testing resizing array based stack.\n";
    auto stack3 = std::make_shared<ResizingArrayStack<int> >();
    testStackImpl<int>(stack3);
}

#endif //ALGS_STACK_H
