#ifndef GRAPH_READER_H
#define GRAPH_READER_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <exception>
#include <sstream>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include "graph.h"

char getDesignator(std::stringstream &ss, int lineNumber)
{
    std::string designatorString;
    ss >> designatorString;

    if (designatorString.length() != 1)
    {
        throw std::logic_error("line " + std::to_string(lineNumber) +
                               " : Designator in beginning of line must be one character.");
    }
    return designatorString[0];
}

template <typename T, typename U, template <typename, typename> typename EdgeType>
Graph<T, U> readGraph(std::ifstream &fileStream)
{

    std::string line;
    int vertex_ct{-1};
    int edge_ct{-1};

    int lineNumber{0};
    char designator{'\0'};
    std::string designatorString;

    // Read lines until the first "Problem line"
    while (designator != 'p' && getline(fileStream, line))
    {
        lineNumber++;
        std::stringstream ss(line);

        designator = getDesignator(ss, lineNumber);

        switch (designator)
        {
        case 'c':
            break;
        case 'n':
        case 'a':
            throw std::logic_error("line " + std::to_string(lineNumber) +
                                   " : node or arc lines should not appear before problem line.");
        case 'p':
        {
            std::string problemDesignator;
            ss >> problemDesignator >> vertex_ct >> edge_ct;

            if (ss.fail())
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       " : Problem line incorrectly formatted");
            }

            if (problemDesignator != "max")
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       " : Not max flow problem");
            }

            if (vertex_ct < 0 || edge_ct < 0)
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       "Number of vertices and edges should be non-negative");
            }

            break;
        }

        default:
            throw std::logic_error("line " + std::to_string(lineNumber) +
                                   " : Unknown designator");
        }
    }

    // Read lines, until the two node lines are read.

    int source{-1};
    int sink{-1};

    while ((source == -1 or sink == -1) && getline(fileStream, line))
    {
        lineNumber++;
        std::stringstream ss(line);

        designator = getDesignator(ss, lineNumber);

        switch (designator)
        {
        case 'c':
            break;

        case 'n':
        {
            int id;
            std::string which;
            ss >> id >> which;

            // DIMACS format is 1-indexed. Transform to 0-indexed:
            --id;

            if (ss.fail())
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       " : Node line incorrectly formatted");
            }

            if (!which.compare("s") && source == -1)
            {
                source = id;
            }
            else if (!which.compare("t") && sink == -1)
            {
                sink = id;
            }
            else
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       " : error in node line.");
            }
            break;
        }

        case 'a':
            throw std::logic_error("line " + std::to_string(lineNumber) +
                                   " : Expected two node lines before the first arc line.");

        case 'p':

            throw std::logic_error("line " + std::to_string(lineNumber) +
                                   " : More than one problem line is not expected.");

        default:
            throw std::logic_error("line " + std::to_string(lineNumber) +
                                   " : Unknown designator");
        }
    }

    if (source >= vertex_ct || sink >= vertex_ct)
    {
        throw std::logic_error("source/sink node can not be larger than vertex count.");
    }

    if (source < 0 || sink < 0)
    {
        throw std::logic_error("source node and/or sink node not set correctly.");
    }

    Graph<T, U> graph(vertex_ct, edge_ct, source, sink);

    // Read rest of the file, which should include all the arch lines.

    std::vector<std::unordered_set<T>> created_edges(vertex_ct);

    while (getline(fileStream, line))
    {
        lineNumber++;
        std::stringstream ss(line);

        designator = getDesignator(ss, lineNumber);

        switch (designator)
        {
        case 'c':
            break;

        case 'p':

            throw std::logic_error("line " + std::to_string(lineNumber) +
                                   " : More than one problem line is not expected.");

        case 'n':
            throw std::logic_error("line " + std::to_string(lineNumber) +
                                   " : More than two node lines is not expected.");
        case 'a':
        {
            if (--edge_ct < 0)
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       " : More edges than indicated in the problem line");
            }

            T src;
            T dst;
            U cap;
            ss >> src >> dst >> cap;

            if (ss.fail())
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       " : Problem line incorrectly formatted.");
            }

            if (src <= 0 || src > vertex_ct || dst <= 0 || dst > vertex_ct)
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       " : Node numbers not correct.");
            }

            // DIMACS format is 1-indexed. Transform to 0-indexed:
            --src;
            --dst;

            // check if parallel edge exist
            if (created_edges[src].count(dst) != 0)
            {
                throw std::logic_error("line " + std::to_string(lineNumber) +
                                       " : Parallel edges not allowed.");
            }

            // If anti-parallel edge exist: Updage edge.
            if (created_edges[dst].count(src) != 0)
            {
                auto it = find_if(graph.adj_list[src].begin(), graph.adj_list[src].end(),
                                  [dst](auto e)
                                  { return e->v == dst; });

                (*it)->capacity += cap;
            }

            // no parallel or anti_parallel edge exist: create new edge
            else
            {
                graph.addEdge(src, dst, cap);
                created_edges[src].insert(dst);
            }
            break;
        }

        default:
            throw std::logic_error("line " + std::to_string(lineNumber) +
                                   " : Unknown designator.");
        }
    }

    if (edge_ct > 0)
    {
        throw std::logic_error("Less edges than indicated in the problem line.");
    }

    return graph;
}

#endif