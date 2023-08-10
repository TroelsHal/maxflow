#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include "graph_reader.h"
#include <cassert>

class ValidFileTest : public ::testing::Test
{
protected:
    std::ifstream fileStream;

    void openFile(const std::string &testFile)
    {
        fileStream.open(testFile);
        if (!fileStream)
        {
            GTEST_SKIP() << "Unable to open file: " << testFile;
        }
    }
};

template <typename T, typename U>
void checkEdge(const std::shared_ptr<Edge<T, U>> &edge, T v, U capacity, T antiParallelV, U antiParallelCapacity)
{
    EXPECT_EQ(edge->v, v);
    EXPECT_EQ(edge->capacity, capacity);

    // Convert the weak_ptr to a shared_ptr
    auto antiParallelEdge = edge->antiParallel.lock();
    assert(antiParallelEdge && "Failed to lock antiParallel weak_ptr");
    EXPECT_EQ(antiParallelEdge->v, antiParallelV);
    EXPECT_EQ(antiParallelEdge->capacity, antiParallelCapacity);

    // Accessing the original edge through two layers of antiParallel
    auto originalEdge = antiParallelEdge->antiParallel.lock();
    assert(originalEdge && "Failed to lock original weak_ptr through antiParallel");
    EXPECT_EQ(originalEdge->v, v);
}

// Test a graph that does not have antiparallel edges
TEST_F(ValidFileTest, ReadGraphFromValidFile)
{

    openFile("../../test/testdata/valid_files/no_antiparallel.txt");

    auto graph = readGraph<int, int, Edge>(fileStream);
    EXPECT_EQ(graph.source, 0);
    EXPECT_EQ(graph.sink, 5);
    EXPECT_EQ(graph.V, 6);
    EXPECT_EQ(graph.E, 9);

    checkEdge(graph.adj_list[0][0], 1, 16, 0, 0);
    checkEdge(graph.adj_list[0][1], 2, 13, 0, 0);
    checkEdge(graph.adj_list[4][0], 2, 0, 4, 14);
    checkEdge(graph.adj_list[4][1], 5, 4, 4, 0);
    checkEdge(graph.adj_list[4][2], 3, 7, 4, 0);
}

// Test a graph that has antiparallel edges
TEST_F(ValidFileTest, ReadGraphFromValidFileAntiparallel)
{

    openFile("../../test/testdata/valid_files/antiparallel.txt");

    // Assuming your graph has int as vertices and int as edges.
    Graph<int, int> graph = readGraph<int, int, Edge>(fileStream);
    EXPECT_EQ(graph.source, 0);
    EXPECT_EQ(graph.sink, 5);
    EXPECT_EQ(graph.V, 6);
    EXPECT_EQ(graph.E, 10);

    checkEdge(graph.adj_list[0][0], 1, 16, 0, 5);
    checkEdge(graph.adj_list[0][1], 2, 13, 0, 0);
    checkEdge(graph.adj_list[4][0], 2, 0, 4, 14);
    checkEdge(graph.adj_list[4][1], 3, 7, 4, 0);
    checkEdge(graph.adj_list[4][2], 5, 4, 4, 0);
}

// The function readGraph<> is a template function, and if we provide its
// template parameters inside the EXPECT_THROW macro, it would seem to the
// preprocessor as if we are providing four parameters to the EXPECT_THROW macro,
// which would cause an error.
// For this reason, we use this wrapper function to handle readGraph
Graph<int, int> readGraphIntIntEdge(std::ifstream &fileStream)
{
    return readGraph<int, int, Edge>(fileStream);
}

// Using GTEST_SKIP() if the file cannot be opened, the test framework will
// mark the test as skipped if the file cannot be opened, rather than failing
// the test or causing undefined behavior.
class InvalidFileTest : public ::testing::TestWithParam<std::string>
{
protected:
    std::ifstream fileStream;

    void SetUp() override
    {
        const std::string filePath = GetParam();
        fileStream.open(filePath);
        if (!fileStream)
        {
            GTEST_SKIP() << "Unable to open file: " << filePath;
        }
    }

    void TearDown() override
    {
        if (fileStream)
        {
            fileStream.close();
        }
    }
};

// Function that takes a directory path and returns a container (e.g., std::vector) of file paths.
std::vector<std::string> getTestFiles(const std::string &directoryPath)
{
    std::vector<std::string> filePaths;
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath))
    {
        filePaths.push_back(entry.path().string());
    }
    return filePaths;
}

// Unpacks the container into ::testing::Values.
class DynamicFileValues
{
public:
    template <class Container>
    static ::testing::internal::ParamGenerator<std::string> Generate(const Container &container)
    {
        return ::testing::ValuesIn(container.begin(), container.end());
    }
};

INSTANTIATE_TEST_SUITE_P(InvalidFiles, InvalidFileTest, DynamicFileValues::Generate(getTestFiles("../../test/testdata/invalid_files")));

// Test files that are not valid DIMACS format or not valid max flow problem. They should all throw an execption.
TEST_P(InvalidFileTest, TestInvalidFiles)
{
    EXPECT_THROW(readGraphIntIntEdge(fileStream), std::logic_error);
}
