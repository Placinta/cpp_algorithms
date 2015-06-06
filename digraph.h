//
// Created by Placinta on 6/4/15.
//

#ifndef ALGS_DIGRAPH_H
#define ALGS_DIGRAPH_H

#include <string>
#include <vector>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include "vendor/transform_output_iterator.hpp"

struct VertexPair : std::tuple<int, int> {};
std::istream& operator>>(std::istream& st, VertexPair& p) {
    st >> std::get<0>(p);
    st >> std::get<1>(p);
    return st;
}

class Digraph {
    using VertexID = int;
    using VertexEdgeList = std::vector<VertexID>;
    using VertexEdgeListIterator = VertexEdgeList::const_iterator;
    using Range = std::pair<VertexEdgeListIterator, VertexEdgeListIterator>;
    using GraphAdjacencyList = std::vector<VertexEdgeList>;
    using GraphAdjacencyListIterator = GraphAdjacencyList::const_iterator;
    using VertexEdgeListAndIndexTuple = boost::tuple<
            GraphAdjacencyListIterator,
            boost::counting_iterator<GraphAdjacencyList::size_type>
    >;
    using VertexEdgeListAndIndexTupleIterator = boost::zip_iterator<VertexEdgeListAndIndexTuple>;

public:
    Digraph(int v) : vertex_count(v) {
        reserveAdjacencyList();
    }

    Digraph(const std::string& filename) {
        std::fstream f(filename, std::fstream::in);
        if (!f) {
            std::cerr << "Error opening file.\n";
            return;
        }

        f >> vertex_count;
        reserveAdjacencyList();

        std::for_each(std::istream_iterator<VertexPair>(f), std::istream_iterator<VertexPair>(), [this](VertexPair p) {
            this->addEdge(std::get<0>(p), std::get<1>(p));
        });
    }

    void addEdge(VertexID v, VertexID w) {
        adjacency_list[v].push_back(w);
        ++edge_count;
    }

    Range adjacent(VertexID v) const {
        return std::make_pair(adjacency_list[v].begin(), adjacency_list[v].end());
    }

    int vertexCount() const { return vertex_count; }
    int edgeCount() const { return edge_count; }
    Digraph reverse() {
        Digraph reversed(vertexCount());

        VertexEdgeListAndIndexTupleIterator begin(VertexEdgeListAndIndexTuple(adjacency_list.begin(), 0));
        VertexEdgeListAndIndexTupleIterator end(VertexEdgeListAndIndexTuple(adjacency_list.end(), adjacency_list.size()));

        std::for_each(begin, end, [&reversed](const boost::tuple<VertexEdgeList, size_t>& adjacentVertices) {
            auto edge_list = adjacentVertices.get<0>();
            auto in = adjacentVertices.get<1>();
            std::for_each(edge_list.begin(), edge_list.end(), [&reversed, &in] (VertexID out) {
                reversed.addEdge(out, (VertexID) in);
            });
        });

        return reversed;
    }

    std::string toString() const {
        std::stringstream ss;

        VertexEdgeListAndIndexTupleIterator begin(VertexEdgeListAndIndexTuple(adjacency_list.begin(), 0));
        VertexEdgeListAndIndexTupleIterator end(VertexEdgeListAndIndexTuple(adjacency_list.end(), adjacency_list.size()));

        std::for_each(begin, end, [&ss](const boost::tuple<VertexEdgeList, size_t>& adjacentVertices) {
            auto edge_list = adjacentVertices.get<0>();
            auto index = adjacentVertices.get<1>();
            std::copy(edge_list.begin(), edge_list.end(), boost::make_transform_output_iterator([index] (VertexID v) {
                return "(" + std::to_string(index) + ", " + std::to_string(v) + ")";
            }, std::ostream_iterator<std::string>(ss, " ")));
        });

        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Digraph& g) {
        os << g.toString();
        return os;
    }

    static int degree(Digraph g, VertexID v) {
        auto range = g.adjacent(v);
        int degree = (int) std::distance(range.first, range.second);
        return degree;
    }

    static int maxDegree(Digraph g) {
        int max = *std::max_element(boost::counting_iterator<VertexID>(0), boost::counting_iterator<VertexID>(g.vertexCount()), [&g] (VertexID a, VertexID b) {
            return Digraph::degree(g, a) > Digraph::degree(g, b);
        });
        return max;
    }

    static double averageDegree(Digraph g) {
        return 2.0f * g.edgeCount() / g.vertexCount();
    }

    static int numberOfSelfLoops(Digraph g) {
        int count = std::accumulate(boost::counting_iterator<VertexID>(0), boost::counting_iterator<VertexID>(g.vertexCount()), 0, [&g] (const int& accumulator, VertexID v) {
            auto range = g.adjacent(v);
            return accumulator + std::count_if(range.first, range.second, [&v] (VertexID w) {
                return v == w;
            });
        });

        return count;
    }

protected:
    void reserveAdjacencyList() {
        adjacency_list.reserve((u_long) vertex_count);
        std::fill_n(std::back_inserter(adjacency_list), vertex_count, VertexEdgeList());
    }

private:
    GraphAdjacencyList adjacency_list;
    int vertex_count = 0;
    int edge_count = 0;
};

class DigraphDFS {
    using BoolVector = std::deque<bool>;
    using VertexID = int;
    using EdgeList = std::vector<VertexID>;
public:
    DigraphDFS(Digraph& _g, int _source) :
            g(_g), source(_source), marked((u_long) g.vertexCount(), false), edge_to((u_long) g.vertexCount(), -1) {
        std::stack<VertexID> frontier;
        frontier.push(source);

        while (!frontier.empty()) {
            auto v = frontier.top();
            frontier.pop();
            marked[v] = true;

            for (auto range = g.adjacent(v); range.first != range.second; ++range.first) {
                auto neighbor_vertex = *(range.first);
                if (!marked[neighbor_vertex]) {
                    frontier.push(neighbor_vertex);
                    edge_to[neighbor_vertex] = v;
                }
            }
        }
    }

    bool hasPathTo(int w) {
        return marked[w];
    }

    EdgeList pathTo(int w) {
        EdgeList path;
        if (!hasPathTo(w)) return path;

        while (w != -1) {
            path.push_back(w);
            w = edge_to[w];
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

    std::string pathToAsString(int w) {
        auto path = pathTo(w);
        std::stringstream ss;
        std::copy(path.begin(), path.end(), std::ostream_iterator<int>(ss, " "));
        return ss.str();
    }

private:
    Digraph g;
    int source;
    BoolVector marked;
    EdgeList edge_to;
};

class DigraphBFS {
    using BoolVector = std::deque<bool>;
    using VertexID = int;
    using EdgeList = std::vector<VertexID>;
public:
    DigraphBFS(Digraph& _g, int source) : DigraphBFS(_g, std::vector<int>{source}) {}

    DigraphBFS(Digraph& _g, const std::vector<int>& multiple_sources) :
            g(_g), marked((u_long) g.vertexCount(), false), edge_to((u_long) g.vertexCount(), -1) {
        std::queue<int> frontier;
        for (auto& elem: multiple_sources) {
            marked[elem] = true;
            frontier.push(std::move(elem));
        }

        while (!frontier.empty()) {
            auto v = frontier.front();
            frontier.pop();

            for (auto range = g.adjacent(v); range.first != range.second; ++range.first) {
                auto adjacent_vertex = *(range.first);
                if (!marked[adjacent_vertex]) {
                    frontier.push(adjacent_vertex);
                    marked[adjacent_vertex] = true;
                    edge_to[adjacent_vertex] = v;
                }
            }
        }
    }

    bool hasPathTo(int w) {
        return marked[w];
    }

    EdgeList pathTo(int w) {
        EdgeList path;
        if (!hasPathTo(w)) return path;

        while (w != -1) {
            path.push_back(w);
            w = edge_to[w];
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

    std::string pathToAsString(int w) {
        auto path = pathTo(w);
        std::stringstream ss;
        std::copy(path.begin(), path.end(), std::ostream_iterator<int>(ss, " "));
        return ss.str();
    }
private:
    Digraph g;
    BoolVector marked;
    EdgeList edge_to;
};

void testDiGraph() {
    Digraph g("data/tinyGraph.txt");
    std::cout << "Directed graph.\n";
    std::cout << g << "\n";
    std::cout << "Vertex count: " << g.vertexCount() << "\n";
    std::cout << "Edge count: " << g.edgeCount() << "\n";
    std::cout << "Number of self loops: " << Digraph::numberOfSelfLoops(g) << "\n";
    std::cout << "Average degree: " << Digraph::averageDegree(g) << "\n";
    std::cout << "Max degree: " << Digraph::maxDegree(g) << "\n";

    auto reversed = g.reverse();
    std::cout << "Reversed graph.\n";
    std::cout << reversed << "\n";

    std::cout << "Depth first search from vertex 1.\n";
    DigraphDFS g_dfs(g, 1);
    for (int i = 0; i < g.vertexCount(); i++) {
        if (g_dfs.hasPathTo(i)) {
            std::cout << g_dfs.pathToAsString(i) << "\n";
        } else {
            std::cout << "No path to vertex " << i << ".\n";
        }
    }

    std::cout << "Breadth first search from vertex 1.\n";
    DigraphBFS bfs(g, 1);
    for (int i = 0; i < g.vertexCount(); i++) {
        if (bfs.hasPathTo(i)) {
            std::cout << bfs.pathToAsString(i) << "\n";
        } else {
            std::cout << "No path to vertex " << i << ".\n";
        }
    }

    std::cout << "Breadth first search from multi-source vertices 0, 1, 2.\n";
    DigraphBFS bfs_multi_source(g, {0, 1, 2});
    for (int i = 0; i < g.vertexCount(); i++) {
        if (bfs_multi_source.hasPathTo(i)) {
            std::cout << bfs_multi_source.pathToAsString(i) << "\n";
        } else {
            std::cout << "No path to vertex " << i << ".\n";
        }
    }
}

#endif //ALGS_DIGRAPH_H
