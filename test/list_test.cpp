#include <gtest/gtest.h>

#include "list.hpp"

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

	list_head<test_class, std::mutex> link{ this };
	using list_type = intrusive_list<test_class, std::mutex, &test_class::link, true, true>;
};

class ListSingleTestFixture : public testing::Test
{
protected:
	void SetUp() override
	{
		for (int i = 0; i <= 10; i++)
		{
			auto t = new test_class{ i };
			list.push_back(t);
		}
	}

	void TearDown() override
	{
		list.clear(true);
	}

	test_class::list_type list;
};

TEST_F(ListSingleTestFixture, Size)
{
	EXPECT_EQ(list.size(), 11);
	EXPECT_EQ(list.size_slow(), 11);
}

TEST_F(ListSingleTestFixture, Clear)
{
	list.clear();

	EXPECT_EQ(list.size(), 0);
	EXPECT_EQ(list.size_slow(), 0);
}

