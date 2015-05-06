//
// Created by Placinta on 5/5/15.
//

#ifndef ALGS_UNIONFIND_H
#define ALGS_UNIONFIND_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "benchmark.h"

class QuickFindUF {
public:
    QuickFindUF (unsigned long n) : size(n) {
        components.reserve(n);
        for (unsigned long i = 0; i < n; i++) {
            components[i] = i;
        }
    }

    void join(unsigned long a, unsigned long b) {
        unsigned long a_component = components[a];
        unsigned long b_component = components[b];
        for (unsigned long i = 0; i < size; i++) {
            if (components[i] == a_component) {
                components[i] = b_component;
            }
        }
    }

    bool connected(unsigned long a, unsigned long b) {
        return components[a] == components[b];
    }

    unsigned long find(unsigned long a) {
        return components[a];
    }

    unsigned long count() {
        return size;
    }

private:
    std::vector<unsigned long> components;
    unsigned long size;
};

class QuickUnionUF {
public:
    QuickUnionUF (unsigned long n) : size(n) {
        parents.reserve(n);
        for (unsigned long i = 0; i < n; i++) {
            parents[i] = i;
        }
    }

    void join(unsigned long a, unsigned long b) {
        unsigned long a_component = root(a);
        unsigned long b_component = root(b);
        parents[a_component] = b_component;
    }

    unsigned long root(unsigned long a) {
        while (a != parents[a]) {
            a = parents[a];
        }
        return a;
    }

    bool connected(unsigned long a, unsigned long b) {
        return root(a) == root(b);
    }

    unsigned long find(unsigned long a) {
        return root(a);
    }

    unsigned long count() {
        return size;
    }

private:
    std::vector<unsigned long> parents;
    unsigned long size;
};

class WeightedQuickUnionUF {
public:
    WeightedQuickUnionUF (unsigned long n) : size(n) {
        parents.reserve(n);
        ranks.reserve(n);
        for (unsigned long i = 0; i < n; i++) {
            parents[i] = i;
            ranks[i] = i;
        }
    }

    void join(unsigned long a, unsigned long b) {
        unsigned long a_component = root(a);
        unsigned long b_component = root(b);
        if (a_component == b_component) return;
        if (ranks[a_component] > ranks[b_component]) {
            parents[b_component] = a_component;
            ranks[a_component] += ranks[b_component];
        }
        else {
            parents[a_component] = b_component;
            ranks[b_component] += ranks[a_component];
        }
        size--;
    }

    unsigned long root(unsigned long a) {
        while (a != parents[a]) {
            parents[a] = parents[parents[a]];
            a = parents[a];
        }
        return a;
    }

    bool connected(unsigned long a, unsigned long b) {
        return root(a) == root(b);
    }

    unsigned long find(unsigned long a) {
        return root(a);
    }

    unsigned long count() {
        return size;
    }

private:
    std::vector<unsigned long> parents;
    std::vector<unsigned long> ranks;
    unsigned long size;
};

template <typename Impl>
int testUFImpl() {
    std::fstream f;
    f.open("data/mediumUF.txt", std::fstream::in);
    if ( (f.rdstate() & std::ifstream::failbit ) != 0 ) {
        std::cerr << "Error opening file.\n";
        return 1;
    }

    std::cout << measure<std::chrono::microseconds>::execution( [&]() {
        unsigned long n;
        f >> n;
        auto uf = Impl(n);

        for (unsigned long i = 0; i < n; i++) {
            unsigned long a, b;
            f >> a >> b;
            uf.join(a, b);
        }

        bool connected = uf.connected(4, 3);
        std::cout << "Connected (3,5): " << connected << std::endl;
    }) << std::endl;


    f.close();
    return 0;
}

int testUF() {
    std::cout << "Testing union find implementations.\n";
    std::cout << "Testing quick find.\n";
    testUFImpl<QuickFindUF>();
    std::cout << "Testing quick union.\n";
    testUFImpl<QuickUnionUF>();
    std::cout << "Testing weighted quick union.\n";
    testUFImpl<WeightedQuickUnionUF>();
    return 0;
}

#endif //ALGS_UNIONFIND_H
