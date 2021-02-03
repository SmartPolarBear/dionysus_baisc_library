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

	auto operator<=>(const avl_test_class& rhs) const
	{
		return value <=> rhs.value;
	}

	int value{ 0 };

	avl_tree_link<avl_test_class, &avl_test_class::value> link{ this };

	using tree_type = avl_tree<avl_test_class, decltype(value), &avl_test_class::value, &avl_test_class::link, false, true>;
};

class AVLTreeSingleTestFixture : public testing::Test
{
protected:
	void SetUp() override
	{
		std::copy(begin(src), end(src), begin(sorted_src));
		std::sort(begin(sorted_src), end(sorted_src));

		for (int i = 0; i <= 10; i++)
		{
			auto t = new avl_test_class{ src[i] };
			tree.insert(t);
		}

		item2 = new avl_test_class(2);
		item114514 = new avl_test_class(114514);
	}

	void TearDown() override
	{
		tree.clear();
	}


	int src[11] = { 2, 0, 1, 3, 9, 4, 20, 2001, 200, 120, 42 };

	int sorted_src[11] = {};

	static constexpr auto SRC_SIZE = sizeof(src) / sizeof(src[0]);


	avl_test_class::tree_type tree;
	avl_test_class::tree_type empty_tree;

	avl_test_class* item2, * item114514;
};

TEST_F(AVLTreeSingleTestFixture, Size)
{
	EXPECT_EQ(tree.size(), 11);
	EXPECT_EQ(empty_tree.size(), 0);

	EXPECT_EQ(tree.empty(), false);
	EXPECT_EQ(empty_tree.empty(), true);
}

TEST_F(AVLTreeSingleTestFixture, Insert)
{
	tree.insert(item2);
	tree.insert(item114514);

	EXPECT_EQ(tree.size(), 12);
}

TEST_F(AVLTreeSingleTestFixture, Removal)
{
	tree.remove(item2);
	EXPECT_EQ(tree.size(), 10);

	tree.remove(item114514); // not exist
	EXPECT_EQ(tree.size(), 10);

}

TEST_F(AVLTreeSingleTestFixture, Clear)
{
	tree.clear();

	EXPECT_EQ(tree.size(), 0);
	EXPECT_EQ(tree.empty(), true);
}

TEST_F(AVLTreeSingleTestFixture, SequentialTraversal)
{
	{
		size_t counter = 0;
		for (auto& item:tree)
		{
			EXPECT_EQ(item.value, sorted_src[counter++]);
		}
	}

	{
		int64_t counter = SRC_SIZE - 1;
		for (auto& item:tree | kbl::reversed)
		{
			EXPECT_EQ(item.value, sorted_src[counter--]);
		}
	}
}

TEST_F(AVLTreeSingleTestFixture, SequentialTraversalWithIterators)
{
	EXPECT_EQ(tree.begin()->value, sorted_src[0]);
	EXPECT_EQ((--tree.end())->value, sorted_src[10]);

	{
		size_t counter = 0;
		for (auto iter = tree.begin(); iter != tree.end(); iter++)
		{
			EXPECT_EQ(iter->value, sorted_src[counter++]);
		}
	}

	{
		int64_t counter = SRC_SIZE - 1;
		for (auto iter = tree.rbegin(); iter != tree.rend(); iter++)
		{
			EXPECT_EQ(iter->value, sorted_src[counter--]);
		}
	}
}