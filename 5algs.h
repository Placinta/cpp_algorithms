//
// Created by Placinta on 5/8/15.
//

#ifndef ALGS_5ALGS_H
#define ALGS_5ALGS_H


#include <vector>
#include <iostream>

int sumRecursive(std::vector<int>& elems, int i) {
    if (i < elems.size()) {
        return elems[i] + sumRecursive(elems, i + 1);
    }
    else return 0;
}

void ex1() {
    std::vector<int> elems = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int sum = 0;
    for (int i = 0; i < elems.size(); i++) {
        sum += elems[i];
    }
    std::cout << "Sum is " << sum << std::endl;
    sum = 0;
    int i = 0;
    while (i < elems.size()) {
        sum += elems[i];
        i++;
    }
    std::cout << "Sum is " << sum << std::endl;
    sum = sumRecursive(elems, 0);
    std::cout << "Sum is " << sum << std::endl;
}

typedef std::vector<std::string> StringVector;

StringVector zipTwo(StringVector l1, StringVector l2) {
    StringVector res;
    auto it1 = l1.begin(), it2 = l2.begin();
    for (; it1 != l1.end() && it2 != l2.end(); it1++, it2++) {
        res.push_back(*it1);
        res.push_back(*it2);
    }
    std::copy(it1, l1.end(), std::back_insert_iterator<StringVector>(res));
    std::copy(it2, l2.end(), std::back_insert_iterator<StringVector>(res));
    return res;
}


void ex2() {
    StringVector list1 = {"a", "b", "c", "d", "e", "f", "g"};
    StringVector list2 = {"1", "2", "3", "4", "5"};
    StringVector res = zipTwo(list1, list2);
    for (auto x = res.begin(); x != res.end(); x++) {
        std::cout << *x << " ";
    }
    std::cout << "\n";
}

long fib_recursive(long n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib_recursive(n-1) + fib_recursive(n-2);
}

u_long fib(u_long n) {
    std::vector<u_long> memo(n + 1, 0);
    u_long i = 0;
    while (i <= n) {
        if (i == 0) {
            memo[i] = 0;
        }
        else if (i == 1) {
            memo[i] = 1;
        }
        else {
            memo[i] = memo[i - 1] + memo[i - 2];
        }
        i++;
    }
    return memo[i - 1];
}

void ex3() {
    for (u_long i = 0; i < 100; i++) {
        std::cout << fib(i) << " ";
    }
    std::cout << std::endl;
}

void ex4() {
    std::vector<int> elems = {5, 50, 56};
    std::sort(elems.begin(), elems.end(), [](int a, int b) {
        return (std::to_string(a) + std::to_string(b)) > std::to_string(b) + std::to_string(a) ;
    });
    for (auto e: elems) {
        std::cout << e;
    }
    std::cout << std::endl;
}

enum OP {PLUS, MINUS, APPEND};
class NumberOp {
public:
    NumberOp(long d, std::string _op) : number(d), op(PLUS) {}
    long number;
    OP op;
};

typedef std::vector<int> VecI;
typedef std::vector<NumberOp> VecNOP;

void printOp(OP op) {
    if (op == PLUS) {
        std::cout << '+';
    }
    else if (op == MINUS) {
        std::cout << '-';
    }

}

long evaluate(VecNOP items) {
    // Squash numbers that are near one another.
    VecNOP new_items;
    for (int i = 0; i < items.size(); i++) {
        int numbers_squashed = 0;
        auto item_i_old = items[i].number;
        auto item_i_op_old = items[i].op;
        while(items[i].op == APPEND && i + numbers_squashed + 1 < items.size()) {
            items[i].number = items[i].number * 10 + items[i + numbers_squashed + 1].number;
            items[i].op = items[i + numbers_squashed + 1].op;
            auto item_i = items[i].number;
            auto item_i_op = items[i].op;
            numbers_squashed++;
        }

        new_items.push_back(items[i]);
        i += numbers_squashed;
    }


    // Compute sum.
    long number = 0;
    for (int i = 0; i < new_items.size(); i++) {
        if (i == 0) {
            number = new_items[i].number;
        }
        else {
            if (new_items[i - 1].op == PLUS) {
                number += new_items[i].number;
            }
            else if (new_items[i - 1].op == MINUS) {
                number -= new_items[i].number;
            }
        }
    }
    return number;
}

void printNumberOPSequence(const VecNOP& elems) {
    int i = 0;
    for (auto e: elems) {
        std::cout << e.number;
        if (e.op != APPEND && i < elems.size() - 1) printOp(e.op);
        i++;
    }
    std::cout << " = " << evaluate(elems) << std::endl;
}

void ex5search(VecNOP& items, int i) {
    if (i == items.size()) {
        long sum = evaluate(items);
        if (sum == 100) {
            printNumberOPSequence(items);
        }
    } else {
        items[i].op = PLUS;
        ex5search(items, i + 1);
        items[i].op = MINUS;
        ex5search(items, i + 1);
        items[i].op = APPEND;
        ex5search(items, i + 1);
    }
}

void ex5() {
    VecI elems = {1,2,3,4,5,6,7,8,9};
    VecNOP items;
    for (auto e: elems) {
        items.push_back(NumberOp(e, "a"));
    }

    ex5search(items, 0);
}

void test5Algs() {
    ex1();
    ex2();
    ex3();
    ex4();
    ex5();
}
#endif //ALGS_5ALGS_H
