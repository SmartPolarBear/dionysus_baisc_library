#pragma once

#include <utility.h>

#include <compare>

namespace kbl
{

template<typename TParent, auto TParent::*Key>
struct avl_tree_link
{
	TParent* owner;

	size_t height;

	avl_tree_link* left, * right, * parent;

	[[nodiscard]] avl_tree_link()
			: owner(nullptr),
			  height(1),
			  left(nullptr),
			  right(nullptr),
			  parent(nullptr)
	{

	}

	[[nodiscard]] explicit avl_tree_link(TParent* on)
			: owner(on),
			  height(1),
			  left(nullptr),
			  right(nullptr),
			  parent(nullptr)
	{

	}
};

template<typename T, auto T::*Key,
		avl_tree_link<T, Key> T::*Link,
		typename TCmp = less<T>,
		bool Reverse = false,
		bool EnableLock = false>
class avl_tree_iterator
{
public:

	template<typename S, auto S::*K,
			avl_tree_link<S, Key> S::*L,
			typename,
			bool>
	class avl_tree;

	using value_type = T;
	using size_type = size_t;
	using ssize_type = int64_t;
	using key_type = decltype(*Key);
	using link_type = avl_tree_link<T, Key>;
	using container_type = avl_tree<T, Key, Link, TCmp, EnableLock>;

	using dummy_type = int;

public:
	avl_tree_iterator() = default;

	explicit avl_tree_iterator(link_type* h) :
			h_(h)
	{
	}

	avl_tree_iterator(avl_tree_iterator
	&& another) noexcept
	{
		h_ = another.h_;
		another.h_ = nullptr;
	}

	avl_tree_iterator(const avl_tree_iterator& another)
	{
		h_ = another.h_;
	}

	avl_tree_iterator& operator=(const avl_tree_iterator& another)
	{
		if (this == &another)return *this;

		h_ = another.h_;
		return *this;
	}


	T& operator*()
	{
		return *operator->();
	}

	T* operator->()
	{
		return h_->parent;
	}

	bool operator==(avl_tree_iterator const& other) const
	{
		return h_ == other.h_;
	}

	bool operator!=(avl_tree_iterator const& other) const
	{
		return !(*this == other);
	}

	avl_tree_iterator& operator++()
	{
		if constexpr (Reverse)
		{
			container_type::avl_prev(h_);
		}
		else
		{
			container_type::avl_next(h_);
		}
		return *this;
	}

	avl_tree_iterator operator++(dummy_type) noexcept
	{
		avl_tree_iterator rc(*this);
		operator++();
		return rc;
	}

	avl_tree_iterator& operator--()
	{
		if constexpr (Reverse)
		{
			container_type::avl_next(h_);
		}
		else
		{
			container_type::avl_prev(h_);
		}
		return *this;
	}

	avl_tree_iterator operator--(dummy_type) noexcept
	{
		avl_tree_iterator rc(*this);
		operator--();
		return rc;
	}

private:
	link_type* h_;
};

template<typename T>
concept AVLTreeKey=
requires(T a, T b)
{
	a <=> b;
};

template<AVLTreeKey T>
struct avl_default_cmp
{
	auto operator()(const T& a, const T& b)
	{
		return a <=> b;
	}
};

template<typename T, AVLTreeKey TKey,
		TKey T::*Key,
		avl_tree_link<T, Key> T::*Link,
		typename TCmp= avl_default_cmp<TKey>,
		bool EnableLock = false>
class avl_tree
{
public:
	using value_type = T;
	using size_type = size_t;
	using ssize_type = int64_t;
	using link_type = avl_tree_link<T, Key>;
	using iterator_type = avl_tree_iterator<T, Key, Link, TCmp, false, EnableLock>;

	template<typename S, auto S::*K,
			avl_tree_link<S, K> S::*L,
			typename C,
			bool,
			bool>
	friend
	class avl_tree_iterator;

	bool insert(T& val)
	{
		if (root_ == nullptr)
		{
			root_ = &(val.*Link);
			return true;
		}

		link_type** newpos = &root_, * parent = nullptr;
		while (*newpos)
		{
			parent = *newpos;
			auto cmp_val = cmp_(val.*Key, (*newpos)->owner->*Key);
			if (cmp_val < 0)
			{
				newpos = &((*newpos)->left);
			}
			else if (cmp_val == 0)
			{
				return false;
			}
			else
			{
				newpos = &((*newpos)->right);
			}
		}

		avl_insert(&(val.*Link), parent, newpos);

		++size_;
		return true;
	}

	bool remove(T& val)
	{
		link_type** newpos = &root_, * parent = nullptr, * node = nullptr;
		while (*newpos)
		{
			parent = *newpos;
			auto cmp_val = cmp_(val->*Key, (*newpos)->owner->*Key);
			if (cmp_val < 0)
			{
				newpos = &((*newpos)->left);
			}
			else if (cmp_val == 0)
			{
				node = *newpos;
			}
			else
			{
				newpos = &((*newpos)->right);
			}
		}

		if (node == nullptr)
		{
			return false;
		}

		avl_remove(node);
		--size_;
		return true;
	}

	[[nodiscard]] size_type size() const
	{
		return size_;
	}

	[[nodiscard]] bool empty() const
	{
		return size_ == 0;
	}

	iterator_type begin()
	{
		return iterator_type{ avl_first(&root_) };
	}

	iterator_type end()
	{
		return iterator_type{ nullptr };
	}

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

	static inline void avl_update_height(link_type* node)
	{
		node->height = std::max(avl_tree_height(node->left), avl_tree_height(node->right)) + 1;
	}

	static inline ssize_type avl_balance_factor(link_type* root)
	{
		if (root == nullptr)
		{
			return 0;
		}

		return avl_tree_height(root->right) - avl_tree_height(root->left);
	}

	static inline link_type* avl_rebalance(link_type* root)
	{
		// find the minimum unbalanced subtree
		auto old_root = root;
		auto bf = avl_balance_factor(root);
		while (root && bf >= -1 && bf <= 1)
		{
			root = root->parent;
			bf = avl_balance_factor(root);
		}

		if (!root) // no need to rebalance
		{
			return old_root;
		}

		auto l_bf = avl_balance_factor(root->left);
		auto r_bf = avl_balance_factor(root->right);

		if (bf > 1 && l_bf > 0) // L
		{
			return avl_left_rotate(root);
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
			return avl_right_rotate(root);
		}
		else
		{
			// should not reach here
			return nullptr;
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
	link_type* root_{ nullptr };
	TCmp cmp_{};
};

}