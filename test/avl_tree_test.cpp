#include <gtest/gtest.h>

#include <avl_tree.h>

#include <algorithm>

using namespace kbl;
using namespace std;

class test_class
{
public:
	test_class() = default;

	test_class(int v) : value(v)
	{
	}

	bool operator<(const test_class& rhs) const
	{
		return value < rhs.value;
	}

	int value{ 0 };

	avl_tree_link<test_class, &test_class::value> link{ this };
	using tree_type = avl_tree<test_class, &test_class::value, &test_class::link>;
};

class AVLTreeSingleTestFixture : public testing::Test
{
protected:
	void SetUp() override
	{
		int a[11] = { 2, 0, 1, 1, 2, 4, 20, 2001, 200, 120, 42 };
		for (int i = 0; i <= 10; i++)
		{
			auto t = new test_class{ a[i] };
			tree.insert(*t);
		}
	}

	void TearDown() override
	{
//		tree.clear(true);
	}

	test_class::tree_type tree;
	test_class::tree_type empty_tree;
};

TEST_F(AVLTreeSingleTestFixture, Size)
{
	EXPECT_EQ(tree.size(), 11);
	EXPECT_EQ(empty_tree.size(), 0);

	EXPECT_EQ(tree.empty(), false);
	EXPECT_EQ(empty_tree.empty(), true);
}
