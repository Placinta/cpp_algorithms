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

template <typename T, typename Compare = std::less<T> >
void sink(std::vector<T>& elements, long i, long n, Compare comp) {
    while((i+1) * 2 - 1 < n) {
        auto index = (i+1) * 2 - 1;
        if (index + 1 < n && comp(elements[index + 1], elements[index])) {
            index++;
        }
        if (comp(elements[index], elements[i])) {
            std::swap(elements[i], elements[index]);
            i = index;
        }
        else break;
    }
}

template <typename T, typename Compare = std::less<T> >
void heap_sort(std::vector<T>& elements, Compare comp) {
    auto n = elements.size();
    auto i = n / 2;

    // Make a heap.
    while (i >= 1) {
        sink(elements, i - 1, n, comp);
        i--;
    }
    print_range(elements.begin(), elements.end());

    // Extract min, put it at end, and sink new min.
    i = n;
    while (i > 1) {
        std::swap(elements[0], elements[i - 1]);
        i--;
        sink(elements, 0, i, comp);
    }

    assert(std::is_sorted(elements.begin(), elements.end(), std::not2(comp)));
}

template <typename T>
void heap_sort(std::vector<T>& elements) {
    heap_sort(elements, std::less<T>());
}


void testPriorityQueue() {
    std::cout << "Test priority queue based on binary heap.\n";
    std::vector<int> elements = { 4, 2, 9, 6, 7, 3, 8, 1, 5};

    auto elements2 = elements;
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

    std::cout << "Test in-place heap construction and heap sort.\n";
    heap_sort(elements2, std::greater<int>());
    print_range(elements2.begin(), elements2.end());
    std::cout << std::endl;
}

#endif //ALGS_PRIORITY_QUEUE_H
