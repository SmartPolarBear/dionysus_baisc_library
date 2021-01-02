#include <iostream>

#include "list.h"

using namespace std;

class test_class
		: public kbl::double_list_node<test_class>
{
public:
	test_class() = default;

	explicit test_class(int v) : value(v)
	{
	}

	int value{ 0 };
};

int main()
{
	kbl::intrusive_double_list<test_class> list;
	auto a = test_class{ 0 };
	auto b = test_class{ 1 };
	auto c = test_class{ 2 };
	auto d = test_class{ 3 };
	auto e = test_class{ 4 };
	auto f = test_class{ 10 };

	list.insert(a);
	list.insert(b);
	list.insert(c);
	list.insert(d);
	list.insert(e);
	list.insert(list.begin(), f);

	for (auto& tc:list)
	{
		std::cout << tc.value << std::endl;
	}

	c.remove_self();
	list.remove(d);

	cout << endl << endl;
	for (auto& tc:list)
	{
		std::cout << tc.value << std::endl;
	}

	std::cout << "Hello, World!" << std::endl;
	return 0;
}
