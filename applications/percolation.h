//
// Created by Placinta on 5/19/15.
//

#ifndef ALGS_PERCOLATION_H
#define ALGS_PERCOLATION_H

#include <assert.h>
#include <deque>
#include <array>
#include "../unionfind.h"

class Percolation {
    // 0 index is top virtual site and site_count + 1 index is bottom virtual site.
    const size_t additional_virtual_site_count = 2;
public:
    Percolation(size_t _n) :
            site_count(_n * _n),
            uf(site_count + additional_virtual_site_count),
            uf_no_virtual_bottom(site_count + additional_virtual_site_count / 2),
            open_sites(site_count + 1, false),
            grid_size(_n) {
        // Connect top sites to virtual top site.
        for (size_t i = 1, j = 1; j <= grid_size; j++) {
            uf.join(topSite(), to1d(i, j));
            uf_no_virtual_bottom.join(topSite(), to1d(i, j));
        }

        // Connect bottom sites to virtual bottom site.
        for (size_t i = grid_size, j = 1; j <= grid_size; j++) {
            uf.join(bottomSite(), to1d(i, j));
        }
    }

    void open(size_t i, size_t j) {
        open_sites[to1d(i, j)] = true;
        open_sites_count++;

        auto neighbor_array = neighbors(i, j);
        size_t i1, j1;

        for (auto neighbor : neighbor_array) {
            std::tie(i1, j1) = neighbor;
            if (validIndex(i1, j1) && isOpen(i1, j1)) {
                uf.join(to1d(i, j), to1d(i1, j1));
                uf_no_virtual_bottom.join(to1d(i, j), to1d(i1, j1));
            }
        }
    }

    bool isOpen(size_t i, size_t j) {
        assert(validIndex(i, j));
        return getOpenSite(i, j) == true;
    }

    bool isFull(size_t i, size_t j) {
        assert(validIndex(i, j));
        assert(isOpen(i, j));
        return uf.connected(topSite(), to1d(i, j));
    }

    bool isFullNoVirtual(size_t i, size_t j) {
        assert(validIndex(i, j));
        assert(isOpen(i, j));
        return uf_no_virtual_bottom.connected(topSite(), to1d(i, j));
    }

    bool percolates() {
        return uf.connected(topSite(), bottomSite());
    }

    void print() {
        for (size_t i = 0; i <= grid_size + 1; i++) {
            for (size_t j = 0; j <= grid_size + 1; j++) {
                if (i == 0 && j == 0) { std::cout << "/"; continue;}
                if (i == grid_size + 1 && j == grid_size + 1) { std::cout << "/"; continue; }
                if (i == 0 && j == grid_size + 1) { std::cout << "\\"; continue; }
                if (i == grid_size + 1 && j == 0) { std::cout << "\\"; continue; }
                if (i == 0 || i == grid_size + 1) { std::cout << "-"; continue; }
                if (j == 0 || j == grid_size + 1) { std::cout << "|"; continue; }

                if (!isOpen(i, j)) { std::cout << "w"; continue; }
                if (isFullNoVirtual(i, j)) std::cout << " ";
                else std::cout << "e";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    size_t getSiteCount() { return site_count; }
    size_t getOpenSiteCount() { return open_sites_count; }

    std::tuple<size_t, size_t> getRandomBlockedSite() {
        std::random_device rd;
        std::mt19937 gen(rd());
        size_t i, j;

        do {
            i = std::uniform_int_distribution<unsigned long long>{1, grid_size}(gen);
            j = std::uniform_int_distribution<unsigned long long>{1, grid_size}(gen);
        }
        while(isOpen(i, j));

        return std::make_tuple(i, j);
    }

protected:
    bool& getOpenSite(size_t i, size_t j) {
        assert(validIndex(i, j));
        return open_sites[to1d(i, j)];
    }

    std::vector<std::tuple<size_t, size_t> > neighbors(size_t i, size_t j) {
        return {{i - 1, j}, {i, j - 1}, {i + 1, j}, {i + 1, j + 1}};
    }

    bool validIndex(size_t i, size_t j) {
        return i >= 1 && i <= grid_size && j >= 1 && j <= grid_size;
    }

    size_t to1d(size_t i, size_t j) {
        assert(validIndex(i, j));
        return (i - 1) * grid_size + j;
    }

    size_t topSite() { return 0; }
    size_t bottomSite() { return site_count + 1; }

private:
    size_t site_count;
    size_t grid_size;
    WeightedQuickUnionUF uf;
    WeightedQuickUnionUF uf_no_virtual_bottom;
    std::deque<bool> open_sites;
    size_t open_sites_count = 0;
};

class PercolationStats {
public:
    /**
     * Perform t percolation experiments on an nxn grid.
     *
     * n - grid size.
     * t - number of independent experiments.
     */
    PercolationStats(size_t n, size_t t) : results(t, 0) {
        std::cout << "Compute percolation.\n";
        for (size_t i = 0; i < t; i++) {
            Percolation p(n);
            simulate(p, i);
        }

        for (size_t i = 0; i < t; i++) {
            _mean += results[i];
        }
        _mean /= t;

        for (size_t i = 0; i < t; i++) {
            _standard_deviation += pow(results[i] - _mean, 2);
        }
        _standard_deviation /= t - 1;

        _confidence_low = _mean - (1.96 * _standard_deviation) / sqrt(t);
        _confidence_high = _mean + (1.96 * _standard_deviation) / sqrt(t);

        printStats();
    }

    void simulate(Percolation& p, size_t experiment_index) {
        auto max_sites = p.getSiteCount();
        while (p.getOpenSiteCount() < max_sites) {
            size_t i, j;
            std::tie(i, j) = p.getRandomBlockedSite();
            p.open(i, j);
            if (p.percolates()) {
                break;
            }
        }
        results[experiment_index] = (double) p.getOpenSiteCount() / max_sites;
    }

    double mean() { return _mean; }
    double standard_deviation() { return _standard_deviation; }
    double confidence_low() { return _confidence_low; }
    double confidence_high() { return _confidence_high; }

    void printStats() {
        std::cout << "Percolation mean: " << mean() << "\n";
        std::cout << "Percolation standard deviation: " << standard_deviation() << "\n";
        std::cout << "Percolation confidence interval: " << confidence_low() << " " << confidence_high() << "\n";
    }

private:
    std::vector<double> results;
    double _mean = 0;
    double _standard_deviation = 0;
    double _confidence_low = 0;
    double _confidence_high = 0;
};

#endif //ALGS_PERCOLATION_H
