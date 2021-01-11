#include <iostream>

#include "intrusive_doubly_linked_list.h"

using namespace kbl;
using namespace std;

class test_class
{
public:
	test_class() = default;

	test_class(int v) : value(v)
	{
	}

	int value{ 0 };

public:
	doubly_linked_node_state<test_class> state1;
	doubly_linked_node_state<test_class> state2;
};

int main()
{
	intrusive_doubly_linked_list<test_class, &test_class::state1> idl1;
	intrusive_doubly_linked_list<test_class, &test_class::state2> idl2;

	test_class nodes[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };

	for (int i = 0; i < 10; i++)
	{
		idl1.push_back(nodes[i]);
		idl2.push_back(nodes[i]);
	}

	cout << "idl1:" << endl;


	for (auto& n:idl1)
	{
		cout << n.value << endl;
	}

	cout << "idl2:" << endl;

	for (auto& n:idl2)
	{
		cout << n.value << endl;
	}

	cout << "erase idl2:" << endl;

	auto iter = idl2.erase(idl2.begin());
	cout << iter->value << endl;

	cout << "idl1:" << endl;


	for (auto& n:idl1)
	{
		cout << n.value << endl;
	}

	cout << "idl2:" << endl;

	for (auto& n:idl2)
	{
		cout << n.value << endl;
	}

	return 0;
}
