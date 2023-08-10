#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace AlgorithmEnums
{

    enum class Algorithm
    {
        ek,
        dinic,
        pr,
        prpar,
    };

    inline std::string enumToString(Algorithm algo)
    {
        switch (algo)
        {
        case Algorithm::ek:
            return "Edmonds-Karp";
        case Algorithm::dinic:
            return "Dinic's algorithm";
        case Algorithm::pr:
            return "Push-Relabel";
        default:
            throw std::invalid_argument("Invalid Algorithm enum");
        }
    }

    inline Algorithm stringToEnum(const std::string &str)
    {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c)
                       { return std::tolower(c); });
        if (lowerStr == "ek")
            return Algorithm::ek;
        else if (lowerStr == "dinic")
            return Algorithm::dinic;
        else if (lowerStr == "pr")
            return Algorithm::pr;
        else
            throw std::invalid_argument(str + " is not a regognized algorithm");
    }
}

#endif // TYPES_H