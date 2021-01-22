#include <iostream>

#include "list.hpp"
#include "avl_tree.h"
#include "hash_table.h"
#include "skip_list.h"
#include "priority_queue.h"

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

	list1.splice(--list1.end(), list2);

	{
		auto t = new test_class{ 20011204 };
		list1.push_front(t);
	}

	{
		auto t = new test_class{ 114514 };
		list1.push_back(t);
	}

	list1.pop_front();
	list1.pop_back();

	for (auto& n:list1)
	{
		cout << n.value << endl;
	}


	return 0;
}
