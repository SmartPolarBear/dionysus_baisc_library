#pragma once

namespace kbl
{

template<typename T>
struct avl_tree_head
{

};

template<typename T, avl_tree_head<T> T::*Link, bool EnableLock = false>
class avl_tree_iterator
{

};

template<typename T, avl_tree_head<T> T::*Link, bool EnableLock = false>
class avl_tree
{
public:
};

}