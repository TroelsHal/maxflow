
#include "command_line_parser.h"
#include "graph_reader.h"
#include "solver.h"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{

    CommandLineParser parser{};

    try
    {
        parser.parse(argc, argv);
    }
    catch (const HelpException &e)
    {
        // The user asked for help, so we exit successfully
        return EXIT_SUCCESS;
    }
    catch (const std::invalid_argument &e)
    {
        // A command line invalid argument was provided
        std::cerr << "Error: " << e.what() << std::endl;
        parser.printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::ifstream fileStream(parser.getFilePath());

    if (!fileStream)
    {
        std::cerr << "Error: could not open or read from file " << parser.getFilePath() << "\n\n";
        parser.printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::unique_ptr<Graph<int, int>> graphPointer;

    try
    {
        // Read the graph data from the file and construct a Graph<int, int> object.
        Graph<int, int> graphObj = readGraph<int, int, Edge>(fileStream);

        // Move the constructed Graph object into a unique_ptr.
        graphPointer = std::make_unique<Graph<int, int>>(std::move(graphObj));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    try
    {
        solve(parser.getAlgorithm(), graphPointer.get());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
