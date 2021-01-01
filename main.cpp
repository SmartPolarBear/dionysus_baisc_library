#include <iostream>

#include "list.h"

class test_class
		: public kbl::double_list_node<test_class*>
{
public:
	test_class() = default;

	explicit test_class(int v) : value(v)
	{
	}

	int value;
};

int main()
{
	kbl::intrusive_double_list<test_class> list;
	list.insert(test_class{ 0 });
	list.insert(test_class{ 1 });
	list.insert(test_class{ 4 });
	list.insert(test_class{ 2 });
	list.insert(test_class{ 3 });

	for (auto& tc:list)
	{
		std::cout << tc.value << std::endl;
	}

	std::cout << "Hello, World!" << std::endl;
	return 0;
}
