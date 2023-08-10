#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <string_view>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <types.h>

class HelpException : public std::exception
{
};

class CommandLineParser
{

public:
    void parse(int argc, char *argv[])
    {
        for (int i{1}; i < argc; ++i)
        {
            if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
            {
                printUsage(argv[0]);
                throw HelpException();
            }
        }

        if (argc < 3)
        {
            throw std::invalid_argument("Algorithm and path must be specified");
        }

        m_algorithm = AlgorithmEnums::stringToEnum(argv[1]);

        m_filePath = argv[2];

        if (argc > 3)
        {
            throw std::invalid_argument("Too many arguments");
        }
    }

    void printUsage(std::string_view program)
    {
        std::cerr << "usage: " << program << " <algorithm> <path>\n\n"
                  << "<algorithm>: Choose one of the following [ek, dinic, pr]\n"
                  << "ek:\tEdmond-Karps algorithm with BFS\n"
                  << "dinic:\tDinic's algorithm\n"
                  << "pr:\tGoldberg & Tarjans push-relabel algorithm with relabel-to-front and gap heuristic\n"
                  << "<path>: Must be a file path to a max flow problem in DIMACS format\n\n"
                  << "-h,--help\t\tShow this help message"
                  << std::endl;
    }

    AlgorithmEnums::Algorithm getAlgorithm() const
    {
        return m_algorithm;
    }

    std::string getFilePath() const
    {
        return m_filePath;
    }

private:
    AlgorithmEnums::Algorithm m_algorithm{};
    std::string m_filePath{};
};

#endif