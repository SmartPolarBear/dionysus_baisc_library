#include <gtest/gtest.h>
#include <mutex>

#include "utility.h"
#include "list.hpp"

using namespace std;
using namespace kbl;

class utility_test_class
{
public:
	utility_test_class() = default;

	explicit utility_test_class(int v) : value(v)
	{
	}

	bool operator<(const utility_test_class &rhs) const
	{
		return value < rhs.value;
	}

	int value{0};

	list_link<utility_test_class, std::mutex> link{this};
	
	using list_type = intrusive_list_with_default_trait<utility_test_class,
														std::mutex,
														&utility_test_class::link,
														true,
														operator_delete_list_deleter<utility_test_class>>;

	using list_type_no_delete = intrusive_list_with_default_trait<utility_test_class,
																  std::mutex,
																  &utility_test_class::link,
																  false>;

};

class ReversedTestFixture : public testing::Test
{
protected:
	void TearDown() final
	{
		list.clear();
	}

	void SetUp() final
	{
		for (int i = 0; i <= 10; i++)
		{
			auto t = new utility_test_class{i};
			list.push_back(t);
		}

	}

	utility_test_class::list_type list;
};

TEST_F(ReversedTestFixture, ReversedRangeBasedLoop)
{
	// by value
	{
		int64_t counter = 10;
		for (const auto &it:reverse(list))
		{
			EXPECT_EQ(it.value, counter--);
		}
	}

	// by reference
	{
		for (auto &it:reverse(list))
		{
			it.value *= 2;
		}

		int64_t counter = 10;
		for (const auto &it:reversed_range(list))
		{
			EXPECT_EQ(it.value, 2 * (counter--));
		}
	}
}

TEST_F(ReversedTestFixture, ReversedRangeBasedLoopWithPipe)
{
	// by value
	{
		int64_t counter = 10;
		for (const auto &it:list | reversed)
		{
			EXPECT_EQ(it.value, counter--);
		}
	}

	// by reference
	{
		for (auto &it:list | reversed)
		{
			it.value *= 2;
		}

		int64_t counter = 10;
		for (const auto &it:reversed_range(list))
		{
			EXPECT_EQ(it.value, 2 * (counter--));
		}
	}
}

