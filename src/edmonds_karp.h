#ifndef EK_H
#define EK_H

#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include <string>
#include <iostream>
#include <stdexcept>
#include <functional>
#include "graph.h"
#include <cassert>

template <typename T, typename U>
class EdmondsKarp
{

public:
    EdmondsKarp(Graph<T, U> &graph) : graph(graph) {}

    U findMaxFlow()
    {
        U maxflow = 0;

        // Initialize parent information vector
        std::vector<ParentInfo<T, U>> parentInfo(graph.V);

        // As long as there are augmenting paths from source to sink
        while (search(parentInfo))
        {
            U pathflow = std::numeric_limits<U>::max();
            T v = graph.sink;
            T u;

            // Find the maximum possible flow in the current path
            while (v != graph.source)
            {
                u = parentInfo[v].u;
                std::shared_ptr<Edge<T, U>> e = parentInfo[v].edge;
                pathflow = std::min(pathflow, e->capacity);
                v = u;
            }

            // Update residual capacities of the edges and reverse edges
            v = graph.sink;
            while (v != graph.source)
            {
                u = parentInfo[v].u;
                std::shared_ptr<Edge<T, U>> edge = parentInfo[v].edge;
                edge->capacity -= pathflow;

                // Convert the weak_ptr to a shared_ptr before accessing
                std::shared_ptr<Edge<T, U>> antiParallelEdge = edge->antiParallel.lock();
                assert(antiParallelEdge && "Failed to lock antiParallel weak_ptr");

                antiParallelEdge->capacity += pathflow;

                v = u;
            }
            // Add path flow to the maximum flow
            maxflow += pathflow;
        }

        // Return the maximum flow at termination
        return maxflow;
    }

private:
    Graph<T, U> &graph;

    bool search(std::vector<ParentInfo<T, U>> &parentInfo)
    {
        std::queue<T> q;

        // Push source to the data structure and mark it as visited
        q.push(graph.source);
        std::vector<bool> visited(graph.V, false);
        visited[graph.source] = true;

        // While there are still nodes to process
        while (!q.empty())
        {
            T u = q.front();
            q.pop();

            // If we've reached the sink, return true
            if (u == graph.sink)
                return true;

            // For every adjacent edge
            for (std::shared_ptr<Edge<T, U>> e : graph.adj_list[u])
            {
                // If not full capacity or we've already visited the node, skip it
                if (e->capacity == 0 or visited[e->v])
                    continue;

                // Otherwise, update parent info, push node to the data structure
                parentInfo[e->v].u = u;
                parentInfo[e->v].edge = e;
                q.push(e->v);
                visited[e->v] = true;
            }
        }

        // If we haven't found a path to the sink, return false
        return false;
    }
};

#endif // EK_H