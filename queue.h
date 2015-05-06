//
// Created by Placinta on 5/6/15.
//

#ifndef ALGS_QUEUE_H
#define ALGS_QUEUE_H

#include <memory>
#include <algorithm>
#include <cassert>
#include "linkedlistnode.h"

template <typename T>
class IQueue {
public:
    virtual void enqueue(T item) = 0;
    virtual T dequeue() = 0;
    virtual bool isEmpty() = 0;
    virtual int size() = 0;

public:
    class IteratorImpl {
    public:
        virtual void increment () = 0;
        virtual bool operator== (const IteratorImpl& other) const = 0;
        virtual bool operator!= (const IteratorImpl& other) const = 0;
        virtual T& operator* () const = 0;
        virtual T& operator-> () const = 0;
        virtual void swap (IteratorImpl& other) = 0;
        virtual IteratorImpl* clone() = 0;
    };

public:
    class ClientIterator : public std::iterator<std::forward_iterator_tag, T> {
        std::unique_ptr<IteratorImpl> impl;

    public:
        ClientIterator(const ClientIterator& other) : impl(other.impl->clone()) {}
        ClientIterator(std::unique_ptr<IteratorImpl> it) : impl(std::move(it)) {}
        void swap(ClientIterator& other) noexcept {
            impl->swap(*(other.impl));
        }

        ClientIterator& operator++ () {
            impl->increment();
            return *this;
        }

        ClientIterator operator++ (int) {
            ClientIterator tmp(*this);
            impl->increment();
            return tmp;
        }

        bool operator== (const ClientIterator& other) const {
            return *impl == *other.impl;
        }

        bool operator!= (const ClientIterator& other) const {
            return *impl != *other.impl;
        }

        T& operator* () const {
            return **impl;
        }

        T& operator-> () const {
            return **impl;
        }
    };

    typedef ClientIterator iterator;

    virtual iterator begin() = 0;
    virtual iterator end() = 0;
};

template <typename T>
class LinkedListQueue : public IQueue<T> {
public:
    LinkedListQueue() : head(nullptr), tail(nullptr), element_count(0) {}

    void enqueue(T item) {
        auto node = std::make_shared<LinkedListNode<T> >(item);
        if (isEmpty()) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        element_count++;
    }

    T dequeue() {
        auto node = head;
        head = head->next;
        element_count--;
        if (isEmpty()) {
            tail = nullptr;
        }
        return node->item;
    }

    bool isEmpty() {
        return size() == 0;
    }

    int size() {
        return element_count;
    }

    class LinkedListForwardIterator : public IQueue<T>::IteratorImpl {

    protected:
        LinkedListForwardIterator(std::shared_ptr<LinkedListNode<T> > initial_node) : node(initial_node) {}
        friend class LinkedListQueue<T>;

    public:
        LinkedListForwardIterator() : node(nullptr) {}

        void swap(typename IQueue<T>::IteratorImpl& other) noexcept {
            LinkedListForwardIterator& concrete_other = dynamic_cast<LinkedListForwardIterator&>(other);
            std::swap(concrete_other.node, node);
        }

        void increment () {
            node = node->next;
        }

        bool operator== (const typename IQueue<T>::IteratorImpl& other) const {
            const LinkedListForwardIterator& concrete_other = dynamic_cast<const LinkedListForwardIterator&>(other);
            return node == concrete_other.node;
        }

        bool operator!= (const typename IQueue<T>::IteratorImpl& other) const {
            const LinkedListForwardIterator& concrete_other = dynamic_cast<const LinkedListForwardIterator&>(other);
            return node != concrete_other.node;
        }

        T& operator* () const {
            assert(node != nullptr && "Invalid iterator dereference!");
            return node->item;
        }

        T& operator-> () const {
            assert(node != nullptr && "Invalid iterator dereference!");
            return node->item;
        }

        LinkedListForwardIterator* clone() {
            return new LinkedListForwardIterator(node);
        }

    private:
        std::shared_ptr<LinkedListNode<T> > node;
    };

    typename IQueue<T>::ClientIterator begin() {
        std::unique_ptr<LinkedListForwardIterator> impl(new LinkedListForwardIterator(head));
        return typename IQueue<T>::iterator(std::move(impl));
    }

    typename IQueue<T>::ClientIterator end() {
        std::unique_ptr<LinkedListForwardIterator> impl(new LinkedListForwardIterator(nullptr));
        return typename IQueue<T>::iterator(std::move(impl));
    }

private:
    std::shared_ptr<LinkedListNode<T> > head;
    std::shared_ptr<LinkedListNode<T> > tail;
    int element_count;
};

template <typename T>
class ResizingArrayQueue : public IQueue<T> {
public:

    ResizingArrayQueue() : elements(std::shared_ptr<T>(new T[10], std::default_delete<T[]>())), element_count(0), capacity(10), head(0), tail(0) {}

    void enqueue(T item) {
        if (element_count == capacity) {
            resize(capacity * 2);
        }
        elements.get()[tail] = item;
        tail = (tail + 1) % capacity;
        element_count++;
    }

    T dequeue() {
        if (element_count > 0 && element_count == capacity / 4) {
            resize(capacity / 2);
        }
        T item = elements.get()[head];
        head = (head + 1) % capacity;
        element_count--;
        return item;
    }

    void resize(int new_capacity) {
        std::shared_ptr<T> new_elements(new T[new_capacity], std::default_delete<T[]>());

        for (int i = head, j = 0; j < element_count; i = (i + 1) % capacity) {
            new_elements.get()[j++] = elements.get()[i];
        }
        std::swap(elements, new_elements);
        capacity = new_capacity;
        head = 0;
        tail = element_count;
    }

    bool isEmpty() {
        return size() == 0;
    }

    int size() {
        return element_count;
    }

    class ArrayForwardIterator : public IQueue<T>::IteratorImpl {

    protected:
        ArrayForwardIterator(std::shared_ptr<T> elems, int s, int c, int cap, int curr = 0) : elements(elems), start(s), count(c), capacity(cap), current_element(curr) {}
        friend class ResizingArrayQueue<T>;

    public:
        ArrayForwardIterator() : start(0), count(0), capacity(0), current_element(0), elements(nullptr) {}

        void swap(typename IQueue<T>::IteratorImpl& other) noexcept {
            ArrayForwardIterator& concrete_other = dynamic_cast<ArrayForwardIterator&>(other);
            std::swap(concrete_other.start, start);
            std::swap(concrete_other.count, count);
            std::swap(concrete_other.capacity, capacity);
            std::swap(concrete_other.current_element, current_element);
        }

        void increment () {
            current_element++;
        }

        bool operator== (const typename IQueue<T>::IteratorImpl& other) const {
            const ArrayForwardIterator& concrete_other = dynamic_cast<const ArrayForwardIterator&>(other);
            return start == concrete_other.start &&
                   count == concrete_other.count &&
                   capacity == concrete_other.capacity &&
                   current_element == concrete_other.current_element;
        }

        bool operator!= (const typename IQueue<T>::IteratorImpl& other) const {
            const ArrayForwardIterator& concrete_other = dynamic_cast<const ArrayForwardIterator&>(other);
            return start != concrete_other.start ||
                   count != concrete_other.count ||
                   capacity != concrete_other.capacity ||
                   current_element != concrete_other.current_element;
        }

        T& operator* () const {
            return elements.get()[(start + current_element) % capacity];
        }

        T& operator-> () const {
            return elements.get()[(start + current_element) % capacity];
        }

        ArrayForwardIterator* clone() {
            return new ArrayForwardIterator(elements, start, capacity, count, current_element);
        }

    private:
        int start;
        int count;
        int capacity;
        int current_element;
        std::shared_ptr<T> elements;
    };

    typename IQueue<T>::ClientIterator begin() {
        std::unique_ptr<ArrayForwardIterator> impl(new ArrayForwardIterator(elements, head, element_count, capacity));
        return typename IQueue<T>::iterator(std::move(impl));
    }

    typename IQueue<T>::ClientIterator end() {
        std::unique_ptr<ArrayForwardIterator> impl(new ArrayForwardIterator(elements, head, element_count, capacity, element_count));
        return typename IQueue<T>::iterator(std::move(impl));
    }

private:
    std::shared_ptr<T> elements;
    int head;
    int tail;
    int element_count;
    int capacity;
};

template <typename T>
void testQueueImpl(std::shared_ptr<IQueue<T> > queue) {
    queue->enqueue(1);
    queue->enqueue(2);
    queue->enqueue(1);
    queue->enqueue(2);
    queue->enqueue(1);
    queue->enqueue(2);

    std::cout << "Size: " << queue->size() << std::endl;

    std::cout << "Iterator behavior check 1st: ";
    for (auto e: *queue) {
        std::cout << e << " ";
    }
    std::cout << std::endl;

    std::cout << "Iterator behavior check 2nd: ";
    for (auto it = queue->begin(); it != queue->end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    while (!queue->isEmpty()) {
        std::cout << queue->dequeue() << std::endl;
    }

    std::cout << "Size: " << queue->size() << std::endl;

    queue->enqueue(3);
    queue->enqueue(4);
    queue->enqueue(5);
    queue->enqueue(6);
    queue->enqueue(7);
    queue->enqueue(8);
    queue->enqueue(9);
    queue->enqueue(10);
    queue->enqueue(11);
    queue->enqueue(12);
    queue->enqueue(13);

    while (!queue->isEmpty()) {
        std::cout << queue->dequeue() << std::endl;
    }
}

void testQueue() {
    std::cout << "Testing queue implementations.\n";
    std::cout << "Testing linked list based queue.\n";
    auto queue = std::make_shared<LinkedListQueue<int> >();
    testQueueImpl<int>(queue);

    std::cout << "Testing resizing array based queue.\n";
    auto queue2 = std::make_shared<ResizingArrayQueue<int> >();
    testQueueImpl<int>(queue2);
}

#endif //ALGS_QUEUE_H
