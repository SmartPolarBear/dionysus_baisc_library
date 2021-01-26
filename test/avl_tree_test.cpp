#include <gtest/gtest.h>

#include <avl_tree.h>

#include <algorithm>

using namespace kbl;
using namespace std;

class avl_test_class
{
public:
	avl_test_class() = default;

	explicit avl_test_class(int v) : value(v)
	{
	}

	bool operator<(const avl_test_class& rhs) const
	{
		return value < rhs.value;
	}

	int value{ 0 };

	avl_tree_link<avl_test_class, &avl_test_class::value> link{ this };

	using tree_type = avl_tree<avl_test_class, decltype(value), &avl_test_class::value, &avl_test_class::link>;
};

class AVLTreeSingleTestFixture : public testing::Test
{
protected:
	void SetUp() override
	{
		int a[11] = { 2, 0, 1, 1, 2, 4, 20, 2001, 200, 120, 42 };
		for (int i = 0; i <= 10; i++)
		{
			auto t = new avl_test_class{ a[i] };
			tree.insert(*t);
		}
	}

	void TearDown() override
	{
//		tree.clear(true);
	}

	avl_test_class::tree_type tree;
	avl_test_class::tree_type empty_tree;
};

TEST_F(AVLTreeSingleTestFixture, Size)
{
	EXPECT_EQ(tree.size(), 11);
	EXPECT_EQ(empty_tree.size(), 0);

	EXPECT_EQ(tree.empty(), false);
	EXPECT_EQ(empty_tree.empty(), true);
}
