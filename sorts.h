//
// Created by Placinta on 5/7/15.
//

#ifndef ALGS_SORTS_H
#define ALGS_SORTS_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <stack>
#include <fstream>

template <typename RandomIt>
void knuth_shuffle(RandomIt first, RandomIt last) {
    std::random_device rd;
    std::mt19937 gen(rd());

    RandomIt i;
    int index = 0, r = 0;
    for (i = first; i != last; i++, index++) {
        r = std::uniform_int_distribution<>{0, index}(gen);
        std::swap(*i, *(first + r));
    };
}


template <typename T>
void selection_sort_vector(std::vector<T>& elements) {
    size_t i, j, min, n = elements.size();
    for (i = 0; i < n; i++) {
        min = i;
        for (j = i + 1; j < n; j++) {
            if (elements[j] < elements[min]) {
                min = j;
            }
        }
        std::swap(elements[i], elements[min]);
    }
}

template <typename It>
void print_range(It first, It last) {
    for (It x = first; x != last; x++) {
        std::cout << *x << " ";
    }
    std::cout << "\n";
}

template <typename RandomIt>
void selection_sort(RandomIt first, RandomIt last) {
    RandomIt i, j, min;
    for (i = first; i != last; i++) {
        min = i;
        for (j = i + 1; j != last; j++) {
            if (*j < *min) {
                min = j;
            }
        }
        std::swap(*i, *min);
    }
}

template <typename RandomIt>
void insertion_sort(RandomIt first, RandomIt last) {
    RandomIt i, j;
    for (i = first; i != last; i++) {
        for (j = i; j != first; j--) { // Does not look at the first element, which is exactly what is needed.
            if (*(j-1) > *j) {
                std::swap(*(j-1), *j);
            }
            else break;
        }
    }
}

template <typename RandomIt>
void shell_sort(RandomIt first, RandomIt last) {
    long n = std::distance(first, last);
    long h = 1;
    while (h < n / 3) {
        h = h * 3 + 1; // Knuth increment.
    }

    while (h >= 1) {
        // h-sort using insertion sort.
        RandomIt i, j;
        for (i = first + h; i != last; i++) {
            for (j = i; j >= first + h; j -= h) {
                if (*(j-h) > *j) {
                    std::swap(*(j-h), *j);
                }
                else break;
            }
        }
        h = h / 3;
    }
}

struct Point2D {
    double x;
    double y;
    Point2D(double x, double y) {
        this->x = x;
        this->y = y;
    }

    static int ccw(Point2D a, Point2D b, Point2D c) {
        double area2 = (b.x-a.x) * (c.y-a.y) - (b.y-a.y) * (c.x-a.x);
        if (area2 < 0) return -1; // clock-wise
        else if (area2 > 0) return 1; // counter-clock-wise
        else return 0; // collinear
    }

    double polarAngle() {
        return atan2(y, x);
    }

    bool operator==(const Point2D& other) const {
        return (fabs(x - other.x) < 0.001) && (fabs(y - other.y)) < 0.001;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point2D& p)
    {
        os << '(' << p.x << ',' << p.y << ')';
        return os;
    }
};
double cross(Point2D p1, Point2D p2) { return p1.x * p2.y - p1.y * p2.x;}

std::stack<Point2D> grahamScanSTL(std::vector<Point2D> points) {
    std::stack<Point2D> hull;

    // Sort by lowest y.
    std::sort(points.begin(), points.end(), [](Point2D a, Point2D b) {return a.y < b.y;});
    print_range(points.begin(), points.end());

    // Sort by polar angle with respect to points[0].
    auto first_point = points[0];
    std::sort(points.begin()+1, points.end(), [&first_point](Point2D p1, Point2D p2) {
        double dx1 = p1.x - first_point.x;
        double dx2 = p2.x - first_point.x;
        double dy1 = p1.y - first_point.y;
        double dy2 = p2.y - first_point.y;
        if (dy1 >= 0 && dy2 < 0) return true;    // q1 above; q2 below
        else if (dy2 >= 0 && dy1 < 0) return false;    // q1 below; q2 above
        else if (dy1 == 0 && dy2 == 0) {            // 3-collinear and horizontal
            if      (dx1 >= 0 && dx2 < 0) return true;
            else if (dx2 >= 0 && dx1 < 0) return false;
            else                          return true;
        }
        else return Point2D::ccw(first_point, p1, p2) >= 0; // return true if p1 is counter-clockwise from p2
    });

    print_range(points.begin(), points.end());

    hull.push(points[0]);
    hull.push(points[1]);

    for (size_t i = 2; i < points.size(); i++) {
        Point2D top = hull.top();
        hull.pop();

        while (Point2D::ccw(hull.top(), top, points[i]) <= 0) {
            top = hull.top();
            hull.pop();
        }
        hull.push(top);
        hull.push(points[i]);
    }

    return hull;
}

void displayPoint2DVector(std::vector<Point2D>& points, long min_x, long max_x, long min_y, long max_y) {
    for (auto x = min_x; x <= max_x; x++) {
        for (auto y = min_y; y <= max_y; y++) {
            auto result = std::find(points.begin(), points.end(), Point2D(x, y));
            if (result != points.end()) {
                std::cout << "p";
            } else {
                std::cout << "_";
            }
        }
        std::cout << "\n";
    }
}

void testGrahamScan() {
    std::cout << "Test Graham Scan.\n";
    std::fstream f;
    f.open("data/input19.txt", std::fstream::in);
    if ( (f.rdstate() & std::ifstream::failbit ) != 0 ) {
        std::cerr << "Error opening file.\n";
        return;
    }

    std::vector<Point2D> points;

    unsigned long n;
    f >> n;
    long min_x = 100, max_x = -100, min_y = 100, max_y = -100;
    for (unsigned long i = 0; i < n; i++) {
        long x, y;
        f >> x >> y;
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
        points.push_back(Point2D(x, y));
    }
    f.close();

    std::cout << "\nGiven points\n";

    displayPoint2DVector(points, min_x, max_x, min_y, max_y);
    std::cout << "\nSort of points by lowest y coordinate, and then by polar angle relative to first point.\n";

    auto hull = grahamScanSTL(points);
    std::vector<Point2D> hull_points;
    while (!hull.empty()) {
        hull_points.push_back(hull.top());
        hull.pop();
    }
    std::cout << "\nConvex hull\n";
    displayPoint2DVector(hull_points, min_x, max_x, min_y, max_y);
}

template <typename RandomIt, typename Container>
void merge_sort_merge(Container aux, RandomIt lo, RandomIt mid, RandomIt hi) {
    auto i = lo, j = mid;
    auto aux_lo = aux.begin(), aux_hi = aux.end();

    // Copy to auxiliary range.
    for (auto k = lo, aux_it = aux_lo; k != hi && aux_it != aux_hi; k++, aux_it++) {
        *aux_it = *k;
    }

    // Get element from auxiliary range, using the index calculated from the iterator into the original range.
    auto getAuxElement = [&aux_lo, &lo](RandomIt it) {
        return *(aux_lo + std::distance(lo, it));
    };

    for (auto k = lo; k != hi; k++) {
        auto i_mid_dist = std::distance(i, mid);
        auto j_hi_dist = std::distance(j, hi);
        // No more elements in left sub-range copy right elements.
        if      (i_mid_dist == 0) { *k = getAuxElement(j); j++; }
        // No more elements in right sub-range copy left elements.
        else if (j_hi_dist == 0)  { *k = getAuxElement(i); i++; }
        // Compare two elements from the auxiliary array, using index computed from original iterator.
        else if (getAuxElement(i) < getAuxElement(j)) { *k = getAuxElement(i); i++; }
        else                      { *k = getAuxElement(j); j++; }
    }
}

template <typename RandomIt, typename Container>
void merge_sort_recursive(Container aux, RandomIt lo, RandomIt hi) {
    long n = std::distance(lo, hi);
    if (n < 2) return;
    auto mid = lo + (n / 2);
    merge_sort_recursive(aux, lo, mid);
    merge_sort_recursive(aux, mid, hi);
    merge_sort_merge(aux, lo, mid, hi);
}

template <typename RandomIt>
void merge_sort(RandomIt first, RandomIt last) {
    std::vector<typename RandomIt::value_type> aux(first, last);
    merge_sort_recursive(aux, first, last);
}

template <typename RandomIt>
void bottom_up_merge_sort(RandomIt first, RandomIt last) {
    std::vector<typename RandomIt::value_type> aux(first, last);
    long n = std::distance(first, last);
    for (size_t sz = 1; sz < n; sz = sz + sz) {
        for (size_t lo = 0; lo < n - sz; lo += sz + sz) {
            merge_sort_merge(aux, first + lo, first + lo + sz, std::min(first + lo + sz + sz, last));
            print_range(first + lo, std::min(first + lo + sz + sz, last));
        }
    }
}

void testSorts() {
    std::vector<int> elements = { 4, 2, 9, 6, 7, 3, 8, 1, 5};
    auto elements2 = elements;
    auto elements3 = elements;
    auto elements4 = elements;
    auto elements5 = elements;
    auto elements6 = elements;
    std::cout << "Test selection sort.\n";
    selection_sort(elements.begin(), elements.end());
    print_range(elements.begin(), elements.end());

    std::cout << "Test selection sort with range.\n";
    selection_sort_vector(elements2);
    print_range(elements2.begin(), elements2.end());

    std::cout << "Test insertion sort.\n";
    insertion_sort(elements3.begin(), elements3.end());
    print_range(elements3.begin(), elements3.end());

    std::cout << "Test shell sort.\n";
    shell_sort(elements4.begin(), elements4.end());
    print_range(elements4.begin(), elements4.end());

    std::cout << "Test knuth shuffle.\n";
    knuth_shuffle(elements4.begin(), elements4.end());
    print_range(elements4.begin(), elements4.end());

    std::cout << "Test merge sort.\n";
    merge_sort(elements5.begin(), elements5.end());
    print_range(elements5.begin(), elements5.end());

    std::cout << "Test bottom up merge sort.\n";
    bottom_up_merge_sort(elements6.begin(), elements6.end());
    print_range(elements6.begin(), elements6.end());
}


#endif //ALGS_SORTS_H
