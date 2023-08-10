#ifndef SOLVER_H
#define SOLVER_READER_H

#include "edmonds_karp.h"
#include "dinic.h"
#include "push_relabel.h"
#include <iostream>
#include <chrono>
#include <types.h>

void printResult(const int maxflowValue, const std::chrono::microseconds algorithmTime, AlgorithmEnums::Algorithm algorithm)
{
    std::cout << "Algorithm used: " << AlgorithmEnums::enumToString(algorithm) << '\n';
    std::cout << "maxflow value: " << maxflowValue << '\n';
    std::cout << "time (microseconds): " << algorithmTime.count() << '\n';
}

template <typename T = int, typename U = int>
void solve(const AlgorithmEnums::Algorithm &algorithm, Graph<T, U> *graph)
{
    auto start = std::chrono::high_resolution_clock::now();
    int maxflowValue{0};
    switch (algorithm)
    {
    case AlgorithmEnums::Algorithm::ek:
    {
        EdmondsKarp<T, U> edmondsKarp(*graph);
        maxflowValue = edmondsKarp.findMaxFlow();
    }
    break;

    case AlgorithmEnums::Algorithm::dinic:
    {
        Dinic<T, U> dinic(*graph);
        maxflowValue = dinic.findMaxFlow();
    }
    break;
    case AlgorithmEnums::Algorithm::pr:
    {
        PushRelabel<T, U> pushRelabel(*graph);
        maxflowValue = pushRelabel.findMaxFlow();
    }
    break;
    default:
        throw std::logic_error("Unknown algorithm");
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto algorithmTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    printResult(maxflowValue, algorithmTime, algorithm);
}

#endif