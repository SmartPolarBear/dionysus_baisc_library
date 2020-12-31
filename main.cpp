#include <iostream>

#include "list.h"

class test_class
		: public kbl::double_list_node<test_class*>
{
public:
	test_class()
	= default;
};

int main()
{
	std::cout << "Hello, World!" << std::endl;
	return 0;
}
