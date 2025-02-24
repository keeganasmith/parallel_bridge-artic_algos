#include "uf.h" 
#include "bridges.h"  
#include <gtest/gtest.h>
#include "articulation_points.h"
using std::cout, std::vector, std::pair, std::make_pair;
// Test fixture for Union_Find
class UnionFindTest : public ::testing::Test {
protected:
    Union_Find uf;

    void SetUp() override {
        uf = Union_Find(10);  // Initialize with 10 elements
    }
};
void print_vector(vector<int>& result){
  for(int i = 0; i < result.size(); i++){
    cout << result[i] << " ";
  }
}

// Test case for the constructor
TEST_F(UnionFindTest, ConstructorInitializesCorrectly) {
    for (size_t i = 0; i < 10; i++) {
        EXPECT_EQ(uf.find(i), i) << "Node " << i << " should be its own parent initially.";
    }
}

// Test case for merging two nodes
TEST_F(UnionFindTest, MergeNodes) {
    EXPECT_TRUE(uf.merge(1, 2)) << "Nodes 1 and 2 should be merged successfully.";
    EXPECT_EQ(uf.find(1), uf.find(2)) << "Nodes 1 and 2 should have the same parent after merging.";
}

// Test case for attempting to merge nodes that are already in the same set
TEST_F(UnionFindTest, MergeNodesAlreadyInSameSet) {
    uf.merge(3, 4);
    EXPECT_FALSE(uf.merge(3, 4)) << "Nodes 3 and 4 are already in the same set, merge should return false.";
}

// Test case for path compression
TEST_F(UnionFindTest, PathCompression) {
    uf.merge(5, 6);
    uf.merge(6, 7);
    EXPECT_EQ(uf.find(5), uf.find(7)) << "Nodes 5 and 7 should have the same parent after merging.";

    size_t root = uf.find(5);
    EXPECT_EQ(uf.find(6), root) << "Path compression should set 6's parent directly to the root.";
    EXPECT_EQ(uf.find(7), root) << "Path compression should set 7's parent directly to the root.";
}

// Test case for rank-based merging
TEST_F(UnionFindTest, MergeByRank) {
    uf.merge(8, 9);
    uf.merge(7, 8);
    EXPECT_EQ(uf.find(7), uf.find(9)) << "Rank-based merging should ensure minimal tree depth.";
}

TEST(FindBridgesTest, SingleBridge) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {1, 3}};  // Bridge: (0, 1)
    vector<pair<int, int>> result = find_bridges(edges, 4);

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], make_pair(0, 1));
}

TEST(FindBridgesTest, MultipleBridges) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}};  // Bridges: (0, 1), (1, 2), (2, 3), (3, 4)
    vector<pair<int, int>> result = find_bridges(edges, 5);

    EXPECT_EQ(result.size(), 4);
    EXPECT_NE(find(result.begin(), result.end(), make_pair(0, 1)), result.end());
    EXPECT_NE(find(result.begin(), result.end(), make_pair(1, 2)), result.end());
    EXPECT_NE(find(result.begin(), result.end(), make_pair(2, 3)), result.end());
    EXPECT_NE(find(result.begin(), result.end(), make_pair(3, 4)), result.end());
}

TEST(FindBridgesTest, NoBridges) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 0}, {1, 3}, {3, 4}, {4, 1}};  // No bridges
    vector<pair<int, int>> result = find_bridges(edges, 5);

    EXPECT_TRUE(result.empty()) << "Expected no bridges, but some were found.";
}

TEST(FindBridgesTest, DisconnectedGraph) {
    vector<pair<int, int>> edges = {{0, 1}, {2, 3}};  // Two disconnected components, each edge is a bridge
    vector<pair<int, int>> result = find_bridges(edges, 4);

    EXPECT_EQ(result.size(), 2);
    EXPECT_NE(find(result.begin(), result.end(), make_pair(0, 1)), result.end());
    EXPECT_NE(find(result.begin(), result.end(), make_pair(2, 3)), result.end());
}

TEST(FindBridgesTest, SingleNodeNoEdges) {
    vector<pair<int, int>> edges = {};  // No edges
    vector<pair<int, int>> result = find_bridges(edges, 1);

    EXPECT_TRUE(result.empty()) << "Expected no bridges for a single node with no edges.";
}

// Helper function to check if two vectors contain the same elements (unordered)
bool compareUnorderedVectors(const vector<int>& a, const vector<int>& b) {
    if (a.size() != b.size()) return false;
    vector<int> sorted_a = a, sorted_b = b;
    std::sort(sorted_a.begin(), sorted_a.end());
    std::sort(sorted_b.begin(), sorted_b.end());
    return sorted_a == sorted_b;
}

// Test case for a graph with a single articulation point
TEST(ArticulationPointsTest, SingleArticulationPoint) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {1, 3}, {3, 4}};  // Articulation Point: {1, 3}
    vector<int> result = articulation_points(edges, 5);

    vector<int> expected = {1, 3}; // Order doesn't matter
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation points {1, 3}";
}

// Test case for a graph with multiple articulation points
TEST(ArticulationPointsTest, MultipleArticulationPoints) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {1, 3}, {3, 4}, {4, 5}, {4, 6}};  
    // Articulation Points: {1, 3, 4}
    vector<int> result = articulation_points(edges, 7);

    vector<int> expected = {1, 3, 4};
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation points {1, 3, 4}";
}

// Test case for a cycle (No articulation points)
TEST(ArticulationPointsTest, NoArticulationPointsCycle) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};  // No articulation points in a cycle
    vector<int> result = articulation_points(edges, 4);

    EXPECT_TRUE(result.empty()) << "Expected no articulation points in a cycle.";
}

// Test case for a fully connected graph (clique)
TEST(ArticulationPointsTest, FullyConnectedGraph) {
    vector<pair<int, int>> edges = {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}};  
    // Fully connected, removing any node still leaves the graph connected
    vector<int> result = articulation_points(edges, 4);

    EXPECT_TRUE(result.empty()) << "Expected no articulation points in a fully connected graph.";
}

// Test case for a disconnected graph
TEST(ArticulationPointsTest, DisconnectedGraph) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {3, 4}};  // Articulation Point: {1}
    vector<int> result = articulation_points(edges, 5);

    vector<int> expected = {1};
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation point {1}";
}

// Test case for a tree structure (every non-leaf node is an articulation point)
TEST(ArticulationPointsTest, TreeGraph) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    // Articulation Points: {1, 3}
    vector<int> result = articulation_points(edges, 6);

    vector<int> expected = {1, 3};
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation points {1, 3}";
}

// Test case for a single node with no edges
TEST(ArticulationPointsTest, SingleNodeNoEdges) {
    vector<pair<int, int>> edges = {};  // No edges
    vector<int> result = articulation_points(edges, 0);

    EXPECT_TRUE(result.empty()) << "Expected no articulation points for a single node with no edges.";
}

// Test case for a simple path graph (all non-leaf nodes are articulation points)
TEST(ArticulationPointsTest, SimplePathGraph) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}};
    // Articulation Points: {1, 2, 3}
    vector<int> result = articulation_points(edges, 5);

    vector<int> expected = {1, 2, 3};
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation points {1, 2, 3}";
}

TEST(ArticulationPointsTest, BiConnectedComponents) {
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 1}, {3, 1}, {0, 5}, {5, 1}, {5, 6}, {6, 1}};
    vector<int> result = articulation_points(edges, 7);
    vector<int> expected = {1};
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation points {1}";
}

TEST(ArticulationPointsTest, Truss){
    vector<pair<int, int>> edges = {{0, 1}, {1, 3}, {0, 3}, {0, 2}, {2, 3}, {3, 4}, {1, 4}};
    vector<int> result = articulation_points(edges, 5);
    vector<int> expected = {};
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation points {}";
}

TEST(ArticulationPointsTest, Truss_plus_one){
    vector<pair<int, int>> edges = {{0, 1}, {1, 3}, {0, 3}, {0, 2}, {2, 3}, {3, 4}, {1, 4}, {5, 0}};
    vector<int> result = articulation_points(edges, 6);
    vector<int> expected = {0};
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation points {0}";
}

TEST(ArticulationPointsTest, Truss_plus_four){
    vector<pair<int, int>> edges = {{0, 1}, {1, 3}, {0, 3}, {0, 2}, {2, 3}, {3, 4}, {1, 4}, {5, 0}, {6, 1}, {7, 4}, {8, 2}};
    vector<int> result = articulation_points(edges, 9);
    vector<int> expected = {0, 1, 2, 4};
    EXPECT_TRUE(compareUnorderedVectors(result, expected)) << "Expected articulation points {0, 1, 2, 4}";
}
