#include <iostream>

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

	list_head<test_class> link{ this };
	using list_type = intrusive_list<test_class, &test_class::link>;
};


int main()
{
	test_class::list_type list1, list2;
	for (int i = 0; i <= 10; i++)
	{
		auto t = new test_class{ i };
		list1.push_back(t);
	}

	for (int i = 0; i <= 10; i++)
	{
		auto t = new test_class{ i * 3 };
		list2.push_back(t);
	}

	list1.merge(list2);

	for (auto& n:list1)
	{
		cout << n.value << endl;
	}

	return 0;
}
