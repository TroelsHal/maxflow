#ifndef PUSH_RELABEL_H
#define PUSH_RELABEL_H

#include "graph.h"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <climits>
#include <cassert>
#include <memory>
#include <algorithm>
#include <cassert>

template <typename T>
struct RTFvertex
{
    T index;
    std::shared_ptr<RTFvertex> next;
};

template <typename T, typename U>
class PushRelabel
{
public:
    // Constructor for the RelabelToFront class, initializes graph
    PushRelabel(Graph<T, U> &graph) : graph(graph) {}

    // Main function to compute the maximum flow using the relabel-to-front algorithm
    U findMaxFlow()
    {
        auto Lhead = initializePreflow();
        currents.resize(graph.V, 0);
        std::shared_ptr<RTFvertex<T>> u = Lhead;
        std::shared_ptr<RTFvertex<T>> uprev = nullptr;
        int oldHeight;

        while (u != nullptr)
        {
            oldHeight = heights[u->index];
            discharge(u);

            // If u was relabelled during discharge: Move u to front of L
            if (heights[u->index] > oldHeight)
            {
                if (u != Lhead)
                {
                    uprev->next = u->next;
                    u->next = Lhead;
                    Lhead = u;
                }
            }

            uprev = u;
            u = u->next;
        }

        return excessflow[graph.sink];
    }

private:
    Graph<T, U> &graph;

    std::vector<int> heightCount;
    std::vector<int> heights;
    std::vector<U> excessflow;
    std::vector<T> currents;

    // Initialize preflow operation, also sets up initial heights and list L
    std::shared_ptr<RTFvertex<T>> initializePreflow()
    {

        // Initialize heights and excess flow
        heights.resize(graph.V, 0);
        excessflow.resize(graph.V, 0);

        // Set source height to number of vertices
        heights[graph.source] = graph.V;

        // Set up gap heuristics.
        heightCount.resize(2 * graph.V, 0);
        heightCount[graph.V] = 1;
        heightCount[0] = graph.V - 1;

        // Initialize excess flow for edges originating from source
        for (auto &e : graph.adj_list[graph.source])
        {
            excessflow[e->v] += e->capacity;
            excessflow[graph.source] -= e->capacity;

            // Convert the weak_ptr to a shared_ptr before accessing
            std::shared_ptr<Edge<T, U>> antiParallelEdge = e->antiParallel.lock();
            assert(antiParallelEdge && "Failed to lock antiParallel weak_ptr");

            antiParallelEdge->capacity += e->capacity;

            e->capacity = 0;
        }

        // Set up list L containing all vertices except source and sink
        std::shared_ptr<RTFvertex<T>> Lhead = nullptr;
        std::shared_ptr<RTFvertex<T>> prevVertex = nullptr;

        for (int i{0}; i < graph.V; ++i)
        {
            if (i == graph.source || i == graph.sink)
                continue;
            std::shared_ptr<RTFvertex<T>> newVertex = std::make_shared<RTFvertex<T>>();
            newVertex->index = i;
            if (!Lhead)
            {
                Lhead = newVertex;
            }
            else
            {
                prevVertex->next = newVertex;
            }
            prevVertex = newVertex;
        }

        return Lhead;
    }

    // Relabel operation: increases height of overflowing vertex
    void relabel(std::shared_ptr<RTFvertex<T>> u)
    {
        // Find minimum height of neighbors
        int minHeight = INT_MAX;
        for (auto e : graph.adj_list[u->index])
        {
            if (e->capacity > 0)
                minHeight = std::min(minHeight, heights[e->v]);
        }

        int k = heights[u->index];
        heights[u->index] = minHeight + 1;

        // Perform the gap relabeling

        // Update heightCount
        heightCount[k]--;
        heightCount[minHeight + 1]++;

        // If a gap is created, update heights of all vertices
        if (heightCount[k] == 0)
        {
            for (int i = 0; i < graph.V; ++i)
            {
                if (heights[i] > k && heights[i] < graph.V)
                {
                    heights[i] = graph.V;
                    heightCount[graph.V]++;
                    heightCount[heights[i]]--;
                }
            }
        }
    }

    // Push operation: push flow from vertex u to its current neighbor
    void push(std::shared_ptr<RTFvertex<T>> u, T current)
    {
        auto delta = std::min(graph.adj_list[u->index][current]->capacity,
                              excessflow[u->index]);
        excessflow[u->index] -= delta;
        excessflow[graph.adj_list[u->index][current]->v] += delta;
        graph.adj_list[u->index][current]->capacity -= delta;

        // Convert the weak_ptr to a shared_ptr before accessing
        std::shared_ptr<Edge<T, U>> antiParallelEdge = graph.adj_list[u->index][current]->antiParallel.lock();
        assert(antiParallelEdge && "Failed to lock antiParallel weak_ptr");
        antiParallelEdge->capacity += delta;
    }

    // Discharge operation: push flow out of vertex until it's no longer overflowing
    void discharge(std::shared_ptr<RTFvertex<T>> u)
    {
        T current = currents[u->index];

        while (excessflow[u->index] > 0)
        {
            if (current == (int)graph.adj_list[u->index].size())
            {
                relabel(u);
                current = 0;
            }
            else if (graph.adj_list[u->index][current]->capacity > 0 and
                     heights[u->index] == heights[graph.adj_list[u->index][current]->v] + 1)
            {
                push(u, current);
            }
            else
            {
                current++;
            }

            currents[u->index] = current;
        }
    }
};

#endif // PUSH_RELABEL_H