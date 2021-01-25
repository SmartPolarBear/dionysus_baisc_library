#pragma once

#include <utility.h>

namespace kbl
{

template<typename TParent, auto TParent::*Key>
struct avl_tree_link
{
	TParent* owner;

	bool is_root;
	size_t height;

	avl_tree_link* left, * right, * parent;

	avl_tree_link()
			: owner(nullptr),
			  is_root(false),
			  height(1),
			  left(nullptr),
			  right(nullptr),
			  parent(nullptr)
	{
	}


};

template<typename T, auto T::*Key,
		avl_tree_link<T, Key> T::*Link,
		bool EnableLock = false>
class avl_tree_iterator
{

};

template<typename T, auto T::*Key,
		avl_tree_link<T, Key> T::*Link,
		typename TCmp= less<T>,
		bool EnableLock = false>
class avl_tree
{
public:
	using value_type = T;
	using size_type = size_t;
	using ssize_type = int64_t;
	using key_type = decltype(*Key);
	using link_type = avl_tree_link<T, Key>;
private:
	static inline link_type* avl_left_rotate(link_type* node)
	{
		auto right = node->right;
		auto tr = right->left;

		right->left = node;
		node->parent = right->left;

		node->right = tr;
		tr->parent = node->right;

		avl_update_height(node);
		avl_update_height(right);

		return right;
	}

	static inline link_type* avl_right_rotate(link_type* node)
	{
		auto left = node->left;
		auto tr = left->right;

		left->right = node;
		node->parent = left->right;

		node->left = tr;
		tr->parent = node->left;

		avl_update_height(node);
		avl_update_height(left);

		return left;
	}

	static inline size_type avl_tree_height(link_type* root)
	{
		return root == nullptr ? 0 : root->height;
	}

	static inline size_type avl_update_height(link_type* node)
	{
		node->height = std::max(avl_tree_height(node->left), avl_tree_height(node->right)) + 1;
	}

	static inline ssize_type avl_balance_factor(link_type* root)
	{
		if (root == nullptr)
		{
			return 0;
		}

		return util_avl_tree_height(root->right) - util_avl_tree_height(root->left);
	}

	static inline link_type* avl_rebalance(link_type* root)
	{
		// find the minimum unbalanced subtree
		auto bf = avl_balance_factor(root);
		while (root && bf >= -1 && bf <= 1)
		{
			root = root->parent;
			bf = avl_balance_factor(root);
		}

		if (!root) // no need to rebalance
		{
			return;
		}

		auto l_bf = avl_balance_factor(root->left);
		auto r_bf = avl_balance_factor(root->right);

		if (bf > 1 && l_bf > 0) // L
		{
			return avl_right_rotate(root);
		}
		else if (bf > 1 && l_bf <= 0) // LR
		{
			root->left = avl_left_rotate(root->left);
			return avl_right_rotate(root);
		}
		else if (bf < -1 && r_bf > 0) // RL
		{
			root->right = avl_right_rotate(root->right);
			return avl_left_rotate(root);
		}
		else if (bf < -1 && r_bf <= 0) //RR
		{
			return avl_left_rotate(root);
		}
		else
		{
			// do nothing
		}
	}

	static inline void avl_insert(link_type* newnode, link_type* parent, link_type** newpos)
	{
		if (newnode->parent != nullptr)
			return;

		newnode->parent = parent;
		*newpos = newnode;

		avl_update_height(parent);
		avl_rebalance(parent);
	}

	static inline void avl_remove(link_type* node)
	{
		if (node->parent == nullptr)
			return;

		link_type** victim = node->parent->left == node ? &node->parent->left : &node->parent->right;

		if (node->left == nullptr && node->right == nullptr)// node is a leave
		{
			*victim = nullptr;
		}
		else if (node->left == nullptr && node->right != nullptr)
		{
			node->right->parent = node->parent;
			*victim = node->right;
		}
		else if (node->left != nullptr && node->right == nullptr)
		{
			node->left->parent = node->parent;
			*victim = node->left;
		}
		else
		{
			link_type* left_max = node->left;
			while (left_max->right)
				left_max = left_max->right;

			avl_remove(left_max); // it's a must that we delete a leaf node

			left_max->right = node->right;
			left_max->left = node->left;
			left_max->parent = node->parent;

			*victim = left_max;

			avl_update_height(left_max);
			avl_rebalance(left_max);
		}

		avl_update_height(node->parent);
		avl_rebalance(node->parent);

		node->parent = nullptr;

		node->height = 1;
	}

	static inline link_type* avl_first(link_type* root)
	{
		if (root->left == nullptr)
			return root;

		auto left = root->left;
		while (left->left)
			left = left->left;

		return left;
	}

	static inline link_type* avl_last(link_type* root)
	{
		if (root->right == nullptr)
			return root;

		auto right = root->right;
		while (right->right)
			right = right->right;

		return right;
	}

	static inline link_type* avl_next(link_type* node)
	{
		if (node->parent == nullptr)return nullptr;

		if (node->right)
		{
			return avl_first(node->right);
		}

		link_type* parent = nullptr;
		while ((parent = node->parent) && node == parent->right)
			node = parent;

		return parent;
	}

	static inline link_type* avl_prev(link_type* node)
	{
		if (node->parent == nullptr)return nullptr;

		if (node->left)
		{
			return avl_last(node->left);
		}

		link_type* parent = nullptr;
		while ((parent = node->parent) && node == parent->left)
			node = parent;

		return parent;
	}


private:
	size_type size_{ 0 };

};

}