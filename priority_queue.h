//
// Created by Placinta on 5/9/15.
//

#ifndef ALGS_PRIORITY_QUEUE_H
#define ALGS_PRIORITY_QUEUE_H

template <typename T, typename Compare = std::less<T> >
class PriorityQueue {
public:
    PriorityQueue() : element_count(0), capacity(5), heap(std::shared_ptr<T>(new T[5], std::default_delete<T[]>())) {}

    template <class It>
    PriorityQueue(It first, It last) : element_count(0), capacity(std::distance(first, last) + 1) {
        heap = std::shared_ptr<T>(new T[capacity], std::default_delete<T[]>());
        insert(first, last);
    }

    void insert(T elem) {
        if (size() + 1 == capacity) {
            resize(capacity * 2);
        }

        long index = size() + 1;
        get(index) = elem;
        increment_size();
        swim(index);
    }

    template <class It>
    void insert(It first, It last) {
        for (auto it = first; it != last; it++) {
            insert(*it);
        }
    }

protected:
    void sink(long k) {
        while (k * 2 <= size()) {
            auto c = k * 2;
            if ((c + 1) <= size() && comp(get(c + 1), get(c))) c++;
            if (comp(get(c), get(k))) {
                std::swap(get(k), get(c));
                k = c;
            }
            else break;
        }
    }

    void swim(long k) {
        while (k > 1) {
            auto p = k / 2;
            if (comp(get(k), get(p))) {
                std::swap(get(k), get(p));
                k = k / 2;
            }
            else break;
        }
    }

public:
    T removeMax() {
        if (size() + 1 == capacity / 4) {
            resize(capacity / 2);
        }

        T max = get(1);
        std::swap(get(1), get(size()));
        decrement_size();
        sink(1);
        return max;
    }

    T peekMax() {
        return get(1);
    }

    long size() { return element_count; }
    long empty() { return size() == 0; }

protected:
    void resize(long new_capacity) {
        std::shared_ptr<T> new_heap(new T[new_capacity], std::default_delete<T[]>());
        for (long i = 1; i <= element_count; i++) {
            new_heap.get()[i] = heap.get()[i];
        }
        std::swap(heap, new_heap);
        capacity = new_capacity;
    }

    void increment_size() { element_count++; }
    void decrement_size() { element_count--; }

    T& get(long index) { return heap.get()[index]; }

    void printHeap() {
        for (long i = 1; i <= size(); i++) {
            std::cout << get(i) << " ";
        }
        std::cout << std::endl;
    }

private:
    std::shared_ptr<T> heap;
    long element_count;
    long capacity;
    Compare comp;
};

void testPriorityQueue() {
    std::vector<int> elements = { 4, 2, 9, 6, 7, 3, 8, 1, 5};
    PriorityQueue<int, std::greater<int> > pq(elements.begin(), elements.end());
    while (!pq.empty()) {
        std::cout << pq.removeMax() << " ";
    }
    std::cout << std::endl;

    pq.insert(elements.begin(), elements.end());
    pq.insert(elements.begin(), elements.end());
    while (!pq.empty()) {
        std::cout << pq.removeMax() << " ";
    }
    std::cout << std::endl;

}

#endif //ALGS_PRIORITY_QUEUE_H
