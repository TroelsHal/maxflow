#ifndef DINIC_H
#define DINIC_H

#include <vector>
#include <queue>
#include <stack>
#include <limits.h>
#include <limits>
#include <string>
#include <iostream>
#include <stdexcept>
#include <functional> // for std::function
#include "graph.h"
#include <memory>
#include <cassert>

template <typename T, typename U>
class Dinic
{
public:
    Dinic(Graph<T, U> &graph) : graph(graph) {}

    U findMaxFlow()
    {
        U maxflow{0};
        std::vector<std::vector<std::shared_ptr<Edge<T, U>>>> levelGraph(graph.V);

        // Initialize parent information vector
        std::vector<ParentInfo<T, U>> parentInfo(graph.V);

        while (findLevelGraph(levelGraph))
        {
            // As long as there are augmenting paths from source to sink in the level-graph
            while (findAugmentingPath(levelGraph, parentInfo))
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
            levelGraph.clear();
            levelGraph.resize(graph.V);
        }

        return maxflow;
    }

private:
    Graph<T, U> &graph;

    bool findLevelGraph(std::vector<std::vector<std::shared_ptr<Edge<T, U>>>> &levelGraph)
    {
        bool sinkFound = false;
        std::vector<T> levels(graph.V, graph.V);
        T currentLevel{0};

        std::queue<T> q;
        q.push(graph.source);
        levels[graph.source] = currentLevel;

        while (!q.empty())
        {
            currentLevel++;
            int k = q.size();
            while (k)
            {
                T u = q.front();
                q.pop();
                k--;

                for (std::shared_ptr<Edge<T, U>> edgePtr : graph.adj_list[u])
                {
                    if (levels[edgePtr->v] >= currentLevel && edgePtr->capacity > 0)
                    {
                        if (edgePtr->v == graph.sink)
                        {
                            sinkFound = true;
                        }

                        else if (levels[edgePtr->v] > currentLevel)
                        {
                            q.push(edgePtr->v);
                        }

                        levels[edgePtr->v] = currentLevel;
                        levelGraph[u].push_back(edgePtr);
                    }
                }
            }
        }

        return sinkFound;
    }

    bool findAugmentingPath(std::vector<std::vector<std::shared_ptr<Edge<T, U>>>> &levelGraph, std::vector<ParentInfo<T, U>> &parentInfo)
    {
        std::stack<T> s;
        std::vector<bool> visited(graph.V, false);

        // Push source to the stack and mark it as visited
        s.push(graph.source);
        visited[graph.source] = true;

        // While there are still vertices to process
        while (!s.empty())
        {
            T u = s.top();
            s.pop();

            // If we've reached the sink, return true
            if (u == graph.sink)
                return true;

            // For every adjacent edge
            for (std::shared_ptr<Edge<T, U>> e : levelGraph[u])
            {
                // If not positive capacity or we've already visited the vertex, skip it
                if (e->capacity == 0 or visited[e->v])
                    continue;

                // Otherwise, update parent info, push node to the data structure
                parentInfo[e->v].u = u;
                parentInfo[e->v].edge = e;
                s.push(e->v);
                visited[e->v] = true;
            }
        }

        // If we haven't found a path to the sink, return false
        return false;
    }
};

#endif // DINIC_H