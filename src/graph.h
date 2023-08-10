#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <memory>

template <typename T, typename U>
struct Edge
{
    T v;
    U capacity;
    std::weak_ptr<Edge<T, U>> antiParallel;

    Edge(T v, U capacity) : v(v), capacity(capacity) {}
};

template <typename T, typename U>
struct ParentInfo
{
    T u;
    std::shared_ptr<Edge<T, U>> edge;
};

template <typename T, typename U>
class Graph
{
public:
    std::vector<std::vector<std::shared_ptr<Edge<T, U>>>> adj_list;
    const T V;
    const int E;
    const T source;
    const T sink;

    Graph(int V, int E, int source, int sink) : adj_list(V), V(V), E(E), source(source), sink(sink) {}

    void addEdge(const T u, const T v, const U cap)
    {
        // Create edge.
        auto edge = std::make_shared<Edge<T, U>>(v, cap);

        // Create corresponding anti-parallel edge.
        auto antiParallelEdge = std::make_shared<Edge<T, U>>(u, 0);

        // Assign anti-parallel edges.
        edge->antiParallel = antiParallelEdge;
        antiParallelEdge->antiParallel = edge;

        // Add edge pointers to adjacency list.
        adj_list[u].push_back(edge);
        adj_list[v].push_back(antiParallelEdge);
    }
};

#endif
