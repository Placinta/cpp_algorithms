//
// Created by Placinta on 5/8/15.
//

#ifndef ALGS_QUEUE_POLICY_BASED_H
#define ALGS_QUEUE_POLICY_BASED_H

template <typename T, typename Policy>
class Queue {
public:
    Queue() : _policy() {}

    void enqueue(T item) {
        _policy.enqueue(std::move(item));
    };

    T dequeue() {
        return _policy.dequeue();
    };

    bool isEmpty() {
        return _policy.isEmpty();
    };

    int size() {
        return _policy.size();
    }

    class iterator : public std::iterator<std::forward_iterator_tag, T> {
        using iterator_type = typename Policy::iterator_type;
        iterator_type impl;

    public:
        iterator(iterator_type it = iterator_type {}) : impl(std::move(it)) {}
        void swap(iterator & other) noexcept {
            impl.swap(other.impl);
        }

        iterator& operator++ () {
            impl.increment();
            return *this;
        }

        iterator operator++ (int) {
            iterator tmp(*this);
            impl.increment();
            return tmp;
        }

        bool operator== (const iterator & other) const {
            return impl == other.impl;
        }

        bool operator!= (const iterator & other) const {
            return impl != other.impl;
        }

        T& operator* () const {
            return *impl;
        }

        T& operator-> () const {
            return *impl;
        }
    };

    iterator begin() { return iterator(_policy.begin()); };
    iterator end() { return iterator(_policy.end()); };

private:
    Policy _policy;
};

template <typename T>
class ResizingArrayPolicy {
public:
    ResizingArrayPolicy() : elements(std::shared_ptr<T>(new T[10], std::default_delete<T[]>())), element_count(0), capacity(10), head(0), tail(0) {}

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

    class ArrayForwardIterator {

    protected:
        ArrayForwardIterator(std::shared_ptr<T> elems, int s, int c, int cap, int curr = 0) : elements(elems), start(s), count(c), capacity(cap), current_element(curr) {}
        friend class ResizingArrayPolicy;

    public:
        ArrayForwardIterator() : start(0), count(0), capacity(0), current_element(0), elements(nullptr) {}

        void swap(ArrayForwardIterator& other) noexcept {
            using std::swap;
            swap(other.start, start);
            swap(other.count, count);
            swap(other.capacity, capacity);
            swap(other.current_element, current_element);
        }

        void increment () {
            current_element++;
        }

        bool operator== (const ArrayForwardIterator other) const {
            return start == other.start &&
                   count == other.count &&
                   capacity == other.capacity &&
                   current_element == other.current_element;
        }

        bool operator!= (const ArrayForwardIterator& other) const {
            return start != other.start ||
                   count != other.count ||
                   capacity != other.capacity ||
                   current_element != other.current_element;
        }

        T& operator* () const {
            return elements.get()[(start + current_element) % capacity];
        }

        T& operator-> () const {
            return elements.get()[(start + current_element) % capacity];
        }

    private:
        int start;
        int count;
        int capacity;
        int current_element;
        std::shared_ptr<T> elements;
    };

    typedef ArrayForwardIterator iterator_type;

    ArrayForwardIterator begin() {
        return ArrayForwardIterator(elements, head, element_count, capacity);
    }

    ArrayForwardIterator end() {
        return ArrayForwardIterator(elements, head, element_count, capacity, element_count);
    }

private:
    std::shared_ptr<T> elements;
    int head;
    int tail;
    int element_count;
    int capacity;
};

template <typename T>
class LinkedListPolicy {
public:
    LinkedListPolicy() : head(nullptr), tail(nullptr), element_count(0) {}

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

    class LinkedListForwardIterator {

    protected:
        LinkedListForwardIterator(std::shared_ptr<LinkedListNode<T> > initial_node) : node(initial_node) {}
        friend class LinkedListPolicy;

    public:
        LinkedListForwardIterator() : node(nullptr) {}

        void swap(LinkedListForwardIterator& other) noexcept {
            using std::swap;
            swap(other.node, node);
        }

        void increment () {
            node = node->next;
        }

        bool operator== (const LinkedListForwardIterator& other) const {
            return node == other.node;
        }

        bool operator!= (const LinkedListForwardIterator& other) const {
            return node != other.node;
        }

        T& operator* () const {
            assert(node != nullptr && "Invalid iterator dereference!");
            return node->item;
        }

        T& operator-> () const {
            assert(node != nullptr && "Invalid iterator dereference!");
            return node->item;
        }

    private:
        std::shared_ptr<LinkedListNode<T> > node;
    };

    typedef LinkedListForwardIterator iterator_type;

    LinkedListForwardIterator begin() {
        return LinkedListForwardIterator(head);
    }

    LinkedListForwardIterator end() {
        return LinkedListForwardIterator(nullptr);
    }

private:
    std::shared_ptr<LinkedListNode<T> > head;
    std::shared_ptr<LinkedListNode<T> > tail;
    int element_count;
};

template<class T>
Queue<T, ResizingArrayPolicy<T> > make_static_resizing_array_queue() {
    return Queue<T, ResizingArrayPolicy<T> >{};
}

template<class T>
Queue<T, LinkedListPolicy<T> > make_static_linked_list_queue() {
    return Queue<T, LinkedListPolicy<T> >{};
}

template <typename T, template<class> class Policy>
void testPolicyBasedQueueImpl(Queue<T, Policy<T> > queue) {
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(1);
    queue.enqueue(2);

    std::cout << "Size: " << queue.size() << std::endl;

    std::cout << "Iterator behavior check 1st: ";
    for (auto e: queue) {
        std::cout << e << " ";
    }
    std::cout << std::endl;

    std::cout << "Iterator behavior check 2nd: ";
    for (auto it = queue.begin(); it != queue.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    while (!queue.isEmpty()) {
        std::cout << queue.dequeue() << std::endl;
    }

    std::cout << "Size: " << queue.size() << std::endl;

    queue.enqueue(3);
    queue.enqueue(4);
    queue.enqueue(5);
    queue.enqueue(6);
    queue.enqueue(7);
    queue.enqueue(8);
    queue.enqueue(9);
    queue.enqueue(10);
    queue.enqueue(11);
    queue.enqueue(12);
    queue.enqueue(13);

    while (!queue.isEmpty()) {
        std::cout << queue.dequeue() << std::endl;
    }
}

void testPolicyBasedQueues() {
    std::cout << "Testing policy based queue implementations.\n";
    std::cout << "Testing linked list based queue.\n";
    auto queue = make_static_linked_list_queue<int>();
    testPolicyBasedQueueImpl<int, LinkedListPolicy>(queue);

    std::cout << "Testing resizing array based queue.\n";
    auto queue2 = make_static_resizing_array_queue<int>();
    testPolicyBasedQueueImpl<int, ResizingArrayPolicy>(queue2);
}

#endif //ALGS_QUEUE_POLICY_BASED_H
