//
// Created by Placinta on 6/12/15.
//

#ifndef ALGS_MAXIMUM_PATH_SUM_H
#define ALGS_MAXIMUM_PATH_SUM_H

#include <fstream>
#include <sstream>
#include <string>

std::pair<int, bool> findMaximumPathSum(std::string filename, bool show_computed_triangle) {
    using RowNumbers = std::vector<int>;
    using JaggedMatrix = std::vector<RowNumbers>;
    JaggedMatrix numbers;
    JaggedMatrix computed_sum;
    std::fstream f(filename, std::fstream::in);
    std::string line;
    if (!f) {
        std::cerr << "Can not open file.\n";
        return std::make_pair(0, false);
    }

    // Read triangle of numbers.
    while (std::getline(f, line)) {
        std::stringstream ss(line);
        RowNumbers row_numbers;
        int number;
        while (ss >> number) {
            row_numbers.push_back(number);
        }
        numbers.push_back(row_numbers);
    }


    // Compute maximum path sum.
    for (size_t i = numbers.size() - 2; i < numbers.size() ; --i) {
        for (size_t j = 0; j < numbers[i].size(); ++j) {
            numbers[i][j] = numbers.at(i).at(j) + std::max(numbers.at(i + 1).at(j), numbers.at(i + 1).at(j + 1));
        }
    }

    if (show_computed_triangle) {
        // Display the sum triangle.
        for (size_t i = 0; i < numbers.size(); ++i) {
            for (size_t j = 0; j < numbers[i].size(); ++j) {
                std::cout << numbers[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    return std::make_pair(numbers.at(0).at(0), true);

}

void testMaximumPathSum() {
    auto pair = findMaximumPathSum("data/euler18.txt", true);
    if (pair.second) {
        std::cout << "Maximum path sum is: " << pair.first << ".\n";
    }

    auto pair2 = findMaximumPathSum("data/euler67.txt", false);
    if (pair2.second) {
        std::cout << "Maximum path sum is: " << pair2.first << ".\n";
    }
}

#endif //ALGS_MAXIMUM_PATH_SUM_H
