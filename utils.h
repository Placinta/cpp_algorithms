//
// Created by Placinta on 5/9/15.
//

#ifndef ALGS_UTILS_H
#define ALGS_UTILS_H

#include <iostream>

template <typename It>
void print_range(It first, It last) {
    for (It x = first; x != last; x++) {
        std::cout << *x << " ";
    }
    std::cout << "\n";
}


template <typename T>
long binary_search_vector(std::vector<T>& elements, T val) {
    typename std::remove_reference<decltype(elements)>::type::size_type n = elements.size();
    if (n < 1) return -1;
    typename std::remove_reference<decltype(elements)>::type::difference_type lo = 0;
    typename std::remove_reference<decltype(elements)>::type::difference_type hi = n - 1;
    while (lo <= hi) {
        typename std::remove_reference<decltype(elements)>::type::difference_type mid = lo + (hi - lo) / 2;
        if (elements[mid] < val) {
            lo = mid + 1;
        }
        else if (elements[mid] > val) {
            hi = mid - 1;
        }
        else return mid;
    }

    return -1;
}

template <typename RandomIt>
RandomIt binary_search_in_range(RandomIt first, RandomIt last, typename RandomIt::value_type val) {
    typename RandomIt::difference_type n = std::distance(first, last);
    if (n < 1) return last;
    auto lo = first;
    auto hi = last - 1;
    while (lo <= hi) {
        auto mid = std::next(lo, std::distance(lo, hi) / 2);
        if (*mid < val) {
            lo = mid + 1;
        }
        else if (*mid > val) {
            hi = mid - 1;
        }
        else return mid;
    }

    return last;
}

void print_numeric_type_sizes() {
    std::cout << "char size " << sizeof(char) << std::endl;
    std::cout << "short size " << sizeof(short) << std::endl;
    std::cout << "Int size " << sizeof(int) << std::endl;
    std::cout << "Long size " << sizeof(long) << std::endl;
    std::cout << "Long long " << sizeof(long long) << std::endl;
    std::cout << "float size " << sizeof(float) << std::endl;
    std::cout << "double size " << sizeof(double) << std::endl;
    std::cout << "size_t size " << sizeof(size_t) << std::endl;
}

#endif //ALGS_UTILS_H
