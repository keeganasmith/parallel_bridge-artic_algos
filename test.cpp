#include "uf.h" 
#include <gtest/gtest.h>

// Test fixture for Union_Find
class UnionFindTest : public ::testing::Test {
protected:
    Union_Find uf;

    void SetUp() override {
        uf = Union_Find(10);  // Initialize with 10 elements
    }
};

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


