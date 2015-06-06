//
// Created by Placinta on 5/21/15.
//

#ifndef ALGS_GRAPH_H
#define ALGS_GRAPH_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <queue>

class Graph {
public:
    typedef std::vector<int> vertexEdgeList;
    typedef vertexEdgeList::const_iterator vertexEdgeListIterator;
    typedef std::pair<vertexEdgeListIterator, vertexEdgeListIterator> Range;
    Graph(int v) : vertex_count(v) {
        for (int i = 0; i < vertex_count; i++) {
            adjacency_list.push_back(vertexEdgeList());
        }
    }

    Graph(std::string file_name) {
        std::fstream f(file_name, std::fstream::in);
        if (!f) {
            std::cerr << "Error opening file.\n";
            return;
        }

        f >> vertex_count;

        for (int i = 0; i < vertex_count; i++) {
            adjacency_list.push_back(vertexEdgeList());
        }

        int i, j;
        while (f >> i >> j) {
            addEdge(i, j);
        }
    }

    void addEdge(int v, int w) {
        adjacency_list[v].push_back(w);
        adjacency_list[w].push_back(v);
        ++edge_count;
        ++edge_count;
    }

    Range adjacent(int v) const {
        auto& vertex_edges = adjacency_list[v];
        return std::make_pair(vertex_edges.begin(), vertex_edges.end());
    }

    vertexEdgeList& adjacentAsVector(int v) { return adjacency_list[v]; }
    int vertexCount() const { return vertex_count; }
    int edgeCount() const { return edge_count; }

    std::string toString() const {
        std::stringstream ss;
        for (int i = 0; i < vertexCount(); ++i) {
            for (auto range = adjacent(i); range.first != range.second; ++range.first) {
                ss << "(" << i << ", " << *range.first << ") ";
            }
        }

        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Graph& g) {
        os << g.toString();
        return os;
    }

    static int degree(Graph g, int v) {
        int degree = 0;
        auto range = g.adjacent(v);
        for (auto it = range.first; it != range.second; it++) ++degree;
        return degree;
    }

    static int maxDegree(Graph g) {
        int max = 0;
        for (int i = 0; i < g.vertexCount(); i++) {
            auto vertex_degree = Graph::degree(g, i);
            if (vertex_degree > max) max = vertex_degree;
        }
        return max;
    }

    static double averageDegree(Graph g) {
        return 2.0f * g.edgeCount() / g.vertexCount();
    }

    static int numberOfSelfLoops(Graph g) {
        int count = 0;
        for (int v = 0; v < g.vertexCount(); v++) {
            auto range = g.adjacent(v);
            for (auto it = range.first; it != range.second; it++) {
                if (v == *it) {
                    ++count;
                }
            }
        }
        return count / 2;
    }

private:
    int vertex_count = 0;
    int edge_count = 0;
    std::vector<vertexEdgeList> adjacency_list;
};

class GraphPaths {
public:
    typedef std::vector<int> Path;
    typedef std::vector<int> VertexVector;
    typedef std::deque<bool> BoolVector;
    GraphPaths(Graph _g, int _source) :
            g(_g), marked(BoolVector((unsigned long) g.vertexCount(), false)), edge_to(VertexVector((unsigned long) g.vertexCount(), -1)), source(_source) {

        std::stack<int> frontier;
        frontier.push(_source);

        while (!frontier.empty()) {
            auto v = frontier.top();
            frontier.pop();
            marked[v] = true;
            for (auto range = g.adjacent(v); range.first != range.second; ++range.first) {
                auto adjacent_vertex = *(range.first);
                if (!marked[adjacent_vertex]) {
                    frontier.push(adjacent_vertex);
                    edge_to[adjacent_vertex] = v;
                }
            }
        }
    }

    bool hasPathTo(int v) {
        return marked[v];
    }

    Path pathTo(int v) {
        Path p;
        p.push_back(v);
        while (v != source) {
            auto parent = edge_to[v];
            p.push_back(parent);
            v = parent;
        }
        std::reverse(p.begin(), p.end());

        return p;
    }

    std::string pathToAsString(int v) {
        auto p = pathTo(v);
        std::stringstream ss;
        std::copy(p.begin(), p.end(), std::ostream_iterator<int>(ss, " "));
        return ss.str();
    }
private:
    Graph g;
    int source;
    BoolVector marked;
    VertexVector edge_to;
};

class GraphBreadthFirstPaths {
public:
    typedef std::vector<int> Path;
    typedef std::vector<int> VertexVector;
    typedef std::vector<int> DistanceVector;
    typedef std::deque<bool> BoolVector;
    GraphBreadthFirstPaths(Graph _g, int _source) :
            g(_g),
            marked(BoolVector((unsigned long) g.vertexCount(), false)),
            edge_to(VertexVector((unsigned long) g.vertexCount(), -1)),
            dist_to(DistanceVector((unsigned long) g.vertexCount(), 0)),
            source(_source) {

        std::queue<int> frontier;
        frontier.push(_source);

        while (!frontier.empty()) {
            auto v = frontier.front();
            frontier.pop();
            marked[v] = true;
            for (auto range = g.adjacent(v); range.first != range.second; ++range.first) {
                auto adjacent_vertex = *(range.first);
                if (!marked[adjacent_vertex]) {
                    frontier.push(adjacent_vertex);
                    edge_to[adjacent_vertex] = v;
                    marked[adjacent_vertex] = true;
                    ++dist_to[adjacent_vertex];
                }
            }
        }
    }

    bool hasPathTo(int v) {
        return marked[v];
    }

    Path pathTo(int v) {
        Path p;
        p.push_back(v);
        while (v != source) {
            auto parent = edge_to[v];
            p.push_back(parent);
            v = parent;
        }
        std::reverse(p.begin(), p.end());

        return p;
    }

    std::string pathToAsString(int v) {
        auto p = pathTo(v);
        std::stringstream ss;
        std::copy(p.begin(), p.end(), std::ostream_iterator<int>(ss, " "));
        return ss.str();
    }
private:
    Graph g;
    int source;
    BoolVector marked;
    VertexVector edge_to;
    DistanceVector dist_to;
};

class GraphConnectedComponents {
public:
    typedef std::vector<int> VertexVector;
    typedef std::deque<bool> BoolVector;

    GraphConnectedComponents(Graph _g) :
            g(_g),
            marked(BoolVector((unsigned long) g.vertexCount(), false)),
            components(VertexVector((unsigned long) g.vertexCount(), 0)) {
        for (int i = 0; i < g.vertexCount(); ++i) {
            components[i] = i;
        }

        for (int i = 0; i < g.vertexCount(); ++i) {
            if (!marked[i]) {
                dfs(i);
                ++component_count;
            }
        }
    }
    bool connected(int v, int w) { return components[v] == components[w]; }
    int count() { return component_count; }
    int id(int v) { return components[v]; }

protected:
    void dfs(int source) {
        std::stack<int> frontier;
        frontier.push(source);

        while (!frontier.empty()) {
            auto v = frontier.top();
            frontier.pop();
            marked[v] = true;
            for (auto range = g.adjacent(v); range.first != range.second; ++range.first) {
                auto adjacent_vertex = *(range.first);
                if (!marked[adjacent_vertex]) {
                    frontier.push(adjacent_vertex);
                    components[adjacent_vertex] = source;
                }
            }
        }
    }
private:
    Graph g;
    int component_count = 0;
    BoolVector marked;
    VertexVector components;
};

class GraphCycle {
public:
    typedef std::vector<int> VertexVector;
    typedef std::deque<bool> BoolVector;

    GraphCycle(Graph _g) :
            g(_g),
            marked(BoolVector((unsigned long) g.vertexCount(), false)),
            edge_to(VertexVector((unsigned long) g.vertexCount(), -1)) {
        if (hasSelfLoop()) return;
        if (hasParallelEdges()) return;

        for (int i = 0; i < g.vertexCount() && !hasCycle(); ++i) {
            if (!marked[i]) {
                dfs(i);
            }
        }
    }

    bool hasCycle() { return has_cycle; }
    VertexVector cycle() { return cycle_vertices; }

    std::string cycleAsString() {
        auto p = cycle();
        std::stringstream ss;
        std::copy(p.begin(), p.end(), std::ostream_iterator<int>(ss, " "));
        return ss.str();
    }

protected:
    bool hasSelfLoop() {
        for (int i = 0; i < g.vertexCount(); ++i) {
            for (auto range = g.adjacent(i); range.first != range.second; ++range.first) {
                auto adjacent_vertex = *(range.first);
                if (adjacent_vertex == i) {
                    cycle_vertices.push_back(i);
                    cycle_vertices.push_back(i);
                    has_cycle = true;
                    return has_cycle;
                }
            }
        }
        return false;
    }

    bool hasParallelEdges() {
        for (int i = 0; i < g.vertexCount(); ++i) {
            for (auto range = g.adjacent(i); range.first != range.second; ++range.first) {
                auto adjacent_vertex = *(range.first);
                if (marked[adjacent_vertex]) {
                    cycle_vertices.push_back(i);
                    cycle_vertices.push_back(adjacent_vertex);
                    cycle_vertices.push_back(i);
                    has_cycle = true;
                    return has_cycle;
                }
                marked[adjacent_vertex] = true;
            }

            for (auto range = g.adjacent(i); range.first != range.second; ++range.first) {
                marked[*(range.first)] = false;
            }
        }

        return false;
    }

    void dfs(int source) {
        typedef struct {
            int v;
            int prev;
        } VertexPair;
        std::stack<VertexPair> frontier;
        frontier.push({source, -1});

        while (!frontier.empty() && !hasCycle()) {
            auto pair = frontier.top();
            frontier.pop();
            marked[pair.v] = true;
            for (auto range = g.adjacent(pair.v); range.first != range.second; ++range.first) {
                auto adjacent_vertex = *(range.first);
                if (!marked[adjacent_vertex]) {
                    frontier.push({adjacent_vertex, pair.v});
                    edge_to[adjacent_vertex] = pair.v;
                }
                else if (adjacent_vertex != pair.prev) {
                    for (auto i = pair.v; i != adjacent_vertex; i = edge_to[i]) {
                        cycle_vertices.push_back(i);
                    }
                    cycle_vertices.push_back(adjacent_vertex);
                    cycle_vertices.push_back(pair.v);
                    has_cycle = true;
                    break;
                }
            }
        }
    }
private:
    Graph g;
    BoolVector marked;
    VertexVector cycle_vertices;
    VertexVector edge_to;
    bool has_cycle = false;
};

void testGraph() {
    Graph g("data/tinyGraph.txt");
    std::cout << g << "\n";
    std::cout << "Number of self loops: " << Graph::numberOfSelfLoops(g) << "\n";
    std::cout << "Average degree: " << Graph::averageDegree(g) << "\n";
    std::cout << "Max degree: " << Graph::maxDegree(g) << "\n";

    std::cout << "Depth first search from vertex 0.\n";
    GraphPaths dfs(g, 0);
    for (int i = 0; i < g.vertexCount(); i++) {
        if (dfs.hasPathTo(i)) {
            std::cout << dfs.pathToAsString(i) << "\n";
        } else {
            std::cout << "No path to vertex " << i << ".\n";
        }
    }

    std::cout << "Breadth first search from vertex 0.\n";
    GraphBreadthFirstPaths bfs(g, 0);
    for (int i = 0; i < g.vertexCount(); i++) {
        if (bfs.hasPathTo(i)) {
            std::cout << bfs.pathToAsString(i) << "\n";
        } else {
            std::cout << "No path to vertex " << i << ".\n";
        }
    }

    GraphConnectedComponents cc(g);
    for (int i = 0; i < g.vertexCount(); i++) {
        std::cout << "Vertex " << i << " connected component: " << cc.id(i) << "\n";
    }
    std::cout << "Connected component count: " << cc.count() << "\n";
    std::cout << "2 connected to 4: " << cc.connected(2, 4) << "\n";
    std::cout << "4 connected to 5: " << cc.connected(4, 5) << "\n";


    GraphCycle gc(g);
    std::cout << "Graph has a cycle: " << gc.hasCycle() << "\n";
    if (gc.hasCycle()) {
        std::cout << gc.cycleAsString() << "\n";
    }
}

#endif //ALGS_GRAPH_H
