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


#endif //ALGS_UTILS_H
