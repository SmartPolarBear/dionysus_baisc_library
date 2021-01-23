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

TEST_F(ListSingleTestFixture, ForIteration)
{
	// i is a copy.
	{
		int counter = 0;
		for (auto i:list)
		{
			EXPECT_EQ(i.value, counter++);
			i.value += 10;
		}

		counter = 0;
		for (auto i:list)
		{
			EXPECT_EQ(i.value, counter++);
		}
	}

	// r is a reference
	{
		int counter = 0;
		for (auto& r:list)
		{
			EXPECT_EQ(r.value, counter++);
			r.value += 10;
		}

		counter = 0;
		for (auto& r:list)
		{
			EXPECT_EQ(r.value, 10 + (counter++));
		}
	}

}
// use iterator

TEST_F(ListSingleTestFixture, IterationByIterators)
{

	int counter = 0;
	for (auto iter = list.begin(); iter != list.end(); iter++)
	{
		EXPECT_EQ(iter->value, counter++);
		iter->value += 10;
	}

	counter = 0;
	for (auto iter = list.begin(); iter != list.end(); iter++)
	{
		EXPECT_EQ(iter->value, (10 + counter++));
	}

}

// reverse
TEST_F(ListSingleTestFixture, IterationByReverseIterators)
{

	int counter = 10;
	for (auto iter = list.rbegin(); iter != list.rend(); iter++)
	{
		EXPECT_EQ(iter->value, counter--);
		iter->value += 999;
	}

	counter = 10;
	for (auto iter = list.rbegin(); iter != list.rend(); iter++)
	{
		EXPECT_EQ(iter->value, (999 + counter--));
	}
}

// const
TEST_F(ListSingleTestFixture, IterationByConstIterators)
{
	int counter = 0;
	for (auto iter = list.cbegin(); iter != list.cend(); iter++)
	{
		EXPECT_EQ(iter->value, counter++);
	}

}


TEST_F(ListSingleTestFixture, Clear)
{
	list.clear();

	EXPECT_EQ(list.size(), 0);
	EXPECT_EQ(list.size_slow(), 0);
	EXPECT_EQ(list.empty(), true);
}

TEST_F(ListSingleTestFixture, Removal)
{
	list.pop_front();
	list.pop_back();

	EXPECT_EQ(list.size(), 9);
	EXPECT_EQ(list.size_slow(), 9);

	EXPECT_EQ(list.begin()->value, 1);
	EXPECT_EQ(list.rbegin()->value, 9);

	{
		auto iter = list.begin();
		iter++;
		iter++;
		iter++;

		EXPECT_EQ(iter->value, 4);
		list.remove(*iter);
		EXPECT_EQ(list.size(), 8);

		iter = list.begin();
		iter++;
		iter++;
		iter++;
		EXPECT_EQ(iter->value, 5);
	}


	{
		auto iter = list.rbegin();
		iter++;

		EXPECT_EQ(iter->value, 8);
		list.remove(*iter);
		EXPECT_EQ(list.size(), 7);

		iter = list.rbegin();
		iter++;
		EXPECT_EQ(iter->value, 7);
	}

	{
		list.erase(list.begin());
		list.erase(list.rbegin());
		EXPECT_EQ(list.size(), 5);
		EXPECT_EQ(list.size_slow(), 5);

		EXPECT_EQ(list.begin()->value, 2);
		EXPECT_EQ(list.rbegin()->value, 7);
	}
}

TEST_F(ListSingleTestFixture, Insert)
{
	list.push_front(new test_class{ 2001 });
	list.push_back(new test_class{ 1204 });

	EXPECT_EQ(list.size(), 13);
	EXPECT_EQ(list.size_slow(), 13);

	EXPECT_EQ(list.begin()->value, 2001);
	EXPECT_EQ(list.rbegin()->value, 1204);
}

TEST_F(ListSingleTestFixture, FrontEnd)
{
	EXPECT_EQ(list.front().value, 0);
	EXPECT_EQ(list.back().value, 10);

	EXPECT_EQ(list.front_ptr()->value, 0);
	EXPECT_EQ(list.back_ptr()->value, 10);
}

