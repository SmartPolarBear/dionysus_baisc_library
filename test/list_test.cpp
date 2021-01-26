#include <gtest/gtest.h>

#include "list.hpp"

#include <algorithm>

using namespace kbl;
using namespace std;

class list_test_class
{
public:
	list_test_class() = default;

	list_test_class(int v) : value(v)
	{
	}

	bool operator<(const list_test_class& rhs) const
	{
		return value < rhs.value;
	}

	int value{ 0 };

	list_link<list_test_class, std::mutex> link{ this };
	using list_type = intrusive_list<list_test_class, std::mutex, &list_test_class::link, true, true>;
	using list_type_no_delete = intrusive_list<list_test_class, std::mutex, &list_test_class::link, true, false>;

};

class ListSingleTestFixture : public testing::Test
{
protected:
	void SetUp() override
	{
		for (int i = 0; i <= 10; i++)
		{
			auto t = new list_test_class{ i };
			list.push_back(t);
		}

		one_element.push_back(new list_test_class{ 20011204 });
	}

	void TearDown() override
	{
		list.clear(true);
	}

	list_test_class::list_type list;
	list_test_class::list_type empty_list;
	list_test_class::list_type_no_delete one_element;
};

class ListMultipleTestFixture : public testing::Test
{
protected:
	void SetUp() override
	{
		for (int i = 0; i < 5; i++)
		{
			auto t = new list_test_class{ 2 * i + 1 };
			list1.push_back(t);
		}

		for (int i = 1; i <= 5; i++)
		{
			auto t = new list_test_class{ 2 * i };
			list2.push_back(t);
		}
	}

	void TearDown() override
	{
		list1.clear(true);
		list2.clear(true);
	}

	list_test_class::list_type list1;
	list_test_class::list_type list2;
};

TEST_F(ListSingleTestFixture, Size)
{
	EXPECT_EQ(list.size(), 11);
	EXPECT_EQ(list.size_slow(), 11);

	EXPECT_EQ(empty_list.size(), 0);
	EXPECT_EQ(empty_list.size_slow(), 0);

}

TEST_F(ListSingleTestFixture, ForIteration)
{
	//empty
	for (auto& i:empty_list)
	{
	}

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

	// empty
	for (auto iter = empty_list.begin(); iter != empty_list.end(); iter++)
	{
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

	// empty
	for (auto iter = empty_list.rbegin(); iter != empty_list.rend(); iter++)
	{
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

	// empty
	for (auto iter = empty_list.cbegin(); iter != empty_list.cend(); iter++)
	{
	}

}


TEST_F(ListSingleTestFixture, Clear)
{
	list.clear();
	empty_list.clear();

	EXPECT_EQ(empty_list.size(), 0);
	EXPECT_EQ(empty_list.size_slow(), 0);

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

	empty_list.pop_front();
	empty_list.pop_back();

	EXPECT_EQ(empty_list.size(), 0);
	EXPECT_EQ(empty_list.size_slow(), 0);

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

TEST_F(ListSingleTestFixture, DoubleRemoval)
{
	auto item = one_element.front_ptr();
	one_element.remove(*item);
	one_element.remove(*item);

	EXPECT_EQ(item->link.next, &item->link);
	EXPECT_EQ(item->link.prev, &item->link);

	EXPECT_EQ(one_element.size(), 0);
	EXPECT_EQ(one_element.size_slow(), 0);
	EXPECT_EQ(one_element.empty(), true);

}

TEST_F(ListSingleTestFixture, Insert)
{
	list.push_front(new list_test_class{ 2001 });
	list.push_back(new list_test_class{ 1204 });

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

TEST_F(ListSingleTestFixture, STLFuncs)
{
	{
//		auto iter = std::find_if(list.begin(), list.end(), [](const list_test_class& t)
//		{
//			return t.value == 0;
//		});
//
//		EXPECT_EQ(iter, list.begin());
	}
}

TEST_F(ListMultipleTestFixture, Merge)
{
	list1.merge(list2);

	EXPECT_EQ(list1.size_slow(), list1.size());
	EXPECT_EQ(list1.size(), 10);

	EXPECT_EQ(list2.size(), list2.size_slow());
	EXPECT_EQ(list2.size_slow(), 0);

	{
		int target_result[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, cnt = 0;
		for (auto& i:list1)
		{
			EXPECT_EQ(i.value, target_result[cnt++]);
		}
	}
}

TEST_F(ListMultipleTestFixture, SpliceFront)
{
	list1.splice(list2);

	EXPECT_EQ(list1.size_slow(), list1.size());
	EXPECT_EQ(list1.size(), 10);

	EXPECT_EQ(list2.size(), list2.size_slow());
	EXPECT_EQ(list2.size_slow(), 0);

	{
		int target_result[] = { 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, }, cnt = 0;
		for (auto& i:list1)
		{
			EXPECT_EQ(i.value, target_result[cnt++]);
		}
	}
}

TEST_F(ListMultipleTestFixture, SpliceOnCertainPos)
{
	list1.splice(++list1.begin(), list2);

	EXPECT_EQ(list1.size_slow(), list1.size());
	EXPECT_EQ(list1.size(), 10);

	EXPECT_EQ(list2.size(), list2.size_slow());
	EXPECT_EQ(list2.size_slow(), 0);

	{
		int target_result[] = { 1, 3, 2, 4, 6, 8, 10, 5, 7, 9, }, cnt = 0;
		for (auto& i:list1)
		{
			EXPECT_EQ(i.value, target_result[cnt++]);
		}
	}
}

