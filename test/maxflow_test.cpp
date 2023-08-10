#include <gtest/gtest.h>
#include "edmonds_karp.h"
#include "dinic.h"
#include "push_relabel.h"
#include "graph_reader.h"
#include <vector>
#include <fstream>

class BasicTests : public ::testing::Test
{
protected:
    std::unique_ptr<Graph<int, int>> graph1;
    std::unique_ptr<Graph<int, int>> graph2;
    std::unique_ptr<Graph<int, int>> graph3;
    std::unique_ptr<Graph<int, int>> graph4;
    std::unique_ptr<Graph<int, int>> graph5;
    std::unique_ptr<Graph<int, int>> graph6;

    void SetUp() override
    {

        // Basic graph
        graph1 = std::make_unique<Graph<int, int>>(4, 5, 0, 3);
        graph1->addEdge(0, 1, 3);
        graph1->addEdge(0, 2, 2);
        graph1->addEdge(1, 2, 5);
        graph1->addEdge(1, 3, 2);
        graph1->addEdge(2, 3, 3);

        // Two vertices, zero edges
        graph2 = std::make_unique<Graph<int, int>>(2, 0, 0, 1);

        // Two vertices, one edge
        graph3 = std::make_unique<Graph<int, int>>(2, 1, 0, 1);
        graph3->addEdge(0, 1, 10);

        // Two vertices, two anti-parallel edges
        graph4 = std::make_unique<Graph<int, int>>(2, 2, 0, 1);
        graph4->addEdge(0, 1, 10);
        graph4->addEdge(1, 0, 20);

        // Source and sink are not the first and the last vertices.
        graph5 = std::make_unique<Graph<int, int>>(4, 5, 3, 0);
        graph5->addEdge(1, 0, 3);
        graph5->addEdge(2, 0, 2);
        graph5->addEdge(2, 1, 5);
        graph5->addEdge(3, 1, 2);
        graph5->addEdge(3, 2, 3);

        // No path from source to sink.
        graph6 = std::make_unique<Graph<int, int>>(4, 3, 0, 3);
        graph6->addEdge(0, 1, 3);
        graph6->addEdge(0, 2, 2);
        graph6->addEdge(1, 2, 5);
        graph6->addEdge(3, 1, 2);
        graph6->addEdge(3, 2, 3);
    }
};

TEST_F(BasicTests, DinicTests)
{

    Dinic<int, int> dinic1(*graph1);
    EXPECT_EQ(dinic1.findMaxFlow(), 5);

    Dinic<int, int> dinic2(*graph2);
    EXPECT_EQ(dinic2.findMaxFlow(), 0);

    Dinic<int, int> dinic3(*graph3);
    EXPECT_EQ(dinic3.findMaxFlow(), 10);

    Dinic<int, int> dinic4(*graph4);
    EXPECT_EQ(dinic4.findMaxFlow(), 10);

    Dinic<int, int> dinic5(*graph5);
    EXPECT_EQ(dinic5.findMaxFlow(), 5);

    Dinic<int, int> dinic6(*graph6);
    EXPECT_EQ(dinic6.findMaxFlow(), 0);
}

TEST_F(BasicTests, EdmondsKarpTests)
{

    EdmondsKarp<int, int> ek1(*graph1);
    EXPECT_EQ(ek1.findMaxFlow(), 5);

    EdmondsKarp<int, int> ek2(*graph2);
    EXPECT_EQ(ek2.findMaxFlow(), 0);

    EdmondsKarp<int, int> ek3(*graph3);
    EXPECT_EQ(ek3.findMaxFlow(), 10);

    EdmondsKarp<int, int> ek4(*graph4);
    EXPECT_EQ(ek4.findMaxFlow(), 10);

    EdmondsKarp<int, int> ek5(*graph5);
    EXPECT_EQ(ek5.findMaxFlow(), 5);

    EdmondsKarp<int, int> ek6(*graph6);
    EXPECT_EQ(ek6.findMaxFlow(), 0);
}

TEST_F(BasicTests, PushRelabelTests)
{

    PushRelabel<int, int> pushRelabel1(*graph1);
    EXPECT_EQ(pushRelabel1.findMaxFlow(), 5);

    PushRelabel<int, int> pushRelabel2(*graph2);
    EXPECT_EQ(pushRelabel2.findMaxFlow(), 0);

    PushRelabel<int, int> pushRelabel3(*graph3);
    EXPECT_EQ(pushRelabel3.findMaxFlow(), 10);

    PushRelabel<int, int> pushRelabel4(*graph4);
    EXPECT_EQ(pushRelabel4.findMaxFlow(), 10);

    PushRelabel<int, int> pushRelabel5(*graph5);
    EXPECT_EQ(pushRelabel5.findMaxFlow(), 5);

    PushRelabel<int, int> pushRelabel6(*graph6);
    EXPECT_EQ(pushRelabel6.findMaxFlow(), 0);
}

class GraphFromFileTest : public ::testing::TestWithParam<std::pair<std::string, int>>
{
protected:
    std::ifstream fileStream;
    std::unique_ptr<Graph<int, int>> graph;
    int expectedMaxFlow;

    void SetUp() override
    {
        auto params = GetParam();
        const std::string filePath = params.first;
        expectedMaxFlow = params.second;
        fileStream.open(filePath);
        if (!fileStream)
        {
            GTEST_SKIP() << "Unable to open file: " << filePath;
        }
        graph = std::make_unique<Graph<int, int>>(readGraph<int, int, Edge>(fileStream));
    }

    void TearDown() override
    {
        if (fileStream)
        {
            fileStream.close();
        }
    }
};

TEST_P(GraphFromFileTest, EdmondsKarpTests)
{
    EdmondsKarp<int, int> ek(*graph);
    int maxflow = ek.findMaxFlow();
    EXPECT_EQ(maxflow, expectedMaxFlow);
}

TEST_P(GraphFromFileTest, DinicsTests)
{
    Dinic<int, int> dinic(*graph);
    int maxflow = dinic.findMaxFlow();
    EXPECT_EQ(maxflow, expectedMaxFlow);
}

TEST_P(GraphFromFileTest, PushRelabelTests)
{
    PushRelabel<int, int> pushRelabel(*graph);
    int maxflow = pushRelabel.findMaxFlow();
    EXPECT_EQ(maxflow, expectedMaxFlow);
}

// Assuming you have a vector of file paths:
std::string folder{"../../test/testdata/maxflowInstances/"};
std::vector<std::pair<std::string, int>> files = {
    {folder + "maxflow20V160E.txt", 73502},
    {folder + "maxflow109V2051E.txt", 199867},
    {folder + "maxflow139V2951E.txt", 132539},
};
INSTANTIATE_TEST_SUITE_P(GraphFromFiles, GraphFromFileTest, ::testing::ValuesIn(files));
