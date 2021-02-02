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

	avl_tree_link* left, * right;

	[[nodiscard]] avl_tree_link()
			: owner(nullptr),
			  height(1),
			  left(nullptr),
			  right(nullptr)
	{

	}

	[[nodiscard]] explicit avl_tree_link(TParent* on)
			: owner(on),
			  height(1),
			  left(nullptr),
			  right(nullptr)
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
	using riterator_type = kbl::reversed_iterator<iterator_type>;

	template<typename S, auto S::*K,
			avl_tree_link<S, K> S::*L,
			typename C,
			bool,
			bool>
	friend
	class avl_tree_iterator;

	void insert(T& val)
	{
		root_ = insert(&(val.*Link), root_);
	}

	void remove(T& val)
	{
		root_ = remove(&(val.*Link), root_);
	}

	void clear()
	{
		for (link_type* victim = max_node(root_); victim; victim = max_node(root_))
		{
			root_ = remove(victim, root_);
		}
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
	static inline constexpr size_type height_of(link_type* node)
	{
		return node == nullptr ? 0 : node->height;
	}

	static inline constexpr TKey& key_of(link_type* node)
	{
		return node->owner->*Key;
	}

	static inline void update_height(link_type* node)
	{
		node->height = std::max(height_of(node->left), height_of(node->right)) + 1;
	}

	static inline constexpr int64_t balance_factor(link_type* node)
	{
		return height_of(node->left) - height_of(node->right);
	}

	static inline link_type* left_rotate(link_type* root)
	{
		auto right = root->right;

		root->right = right->left;
		right->left = root;

		update_height(right);
		update_height(root);

		return right;
	}

	static inline link_type* right_rotate(link_type* root)
	{
		auto left = root->left;

		root->left = left->right;
		left->right = root;

		update_height(root);
		update_height(left);

		return left;
	}

	static inline link_type* min_node(link_type* root)
	{
		if (!root)return root;

		while (root->left)
			root = root->left;

		return root;
	}

	static inline link_type* max_node(link_type* root)
	{
		if (!root)return root;

		while (root->right)
			root = root->right;

		return root;
	}

	inline auto compare(link_type* n1, link_type* n2)
	{
		return cmp_(key_of(n1), key_of(n2));
	}

	link_type* insert(link_type* newnode, link_type* root)
	{
		if (root == nullptr)
		{
			++size_;
			return newnode;
		}

		auto cmp_val = compare(newnode, root);// cmp_(newnode->owner->*Key, root->owner->*Key);
		if (cmp_val < 0)
		{
			root->left = insert(newnode, root->left);
		}
		else if (cmp_val > 0)
		{
			root->right = insert(newnode, root->right);
		}
		else
		{
			return root;
		}

		update_height(root);

		auto bf = balance_factor(root);
		if (bf > 1 && /*cmp_(newnode->owner->*Key, root->left->owner->*Key)*/compare(newnode, root->left) > 0)
		{
			root->left = left_rotate(root->left);
			return right_rotate(root);
		}
		else if (bf > 1 && /*cmp_(newnode->owner->*Key, root->left->owner->*Key)*/compare(newnode, root->left) < 0)
		{
			return right_rotate(root);
		}
		else if (bf < -1 && /*cmp_(newnode->owner->*Key, root->right->owner->*Key)*/compare(newnode, root->right) > 0)
		{
			return left_rotate(root);
		}
		else if (bf < -1 && /*cmp_(newnode->owner->*Key, root->right->owner->*Key)*/ compare(newnode, root->right) < 0)
		{
			root->right = right_rotate(root->right);
			return left_rotate(root);
		}

		return root;
	}

	link_type* do_remove(link_type* node, link_type* root)
	{
		if (root->left == nullptr && root->right == nullptr)
		{
			root = nullptr;
			--size_;
		}
		else if (root->left == nullptr || root->right == nullptr)
		{
			auto newroot = root->left ? root->left : root->right;
			root = newroot;
			--size_;
		}
		else
		{
			auto newroot = min_node(root->right);

			root->right = remove(root->right, newroot);

			newroot->left = root->left;
			newroot->right = root->right;

			root = newroot;
		}

		return root;
	}

	link_type* remove(link_type* node, link_type* root)
	{
		if (root == nullptr)
		{
			return root; // do not exist
		}


		if (auto cmp_val = compare(node, root);cmp_val < 0)
		{
			root->left = remove(node, root->left);
		}
		else if (cmp_val > 0)
		{
			root->right = remove(node, root->right);
		}
		else
		{
			root = do_remove(node, root);
		}

		if (root == nullptr)
		{
			return root;
		}

		update_height(root);

		auto bf = balance_factor(root);

		// Left Left Case
		if (bf > 1 && balance_factor(root->left) >= 0)
		{
			return right_rotate(root);
		}

		// Left Right Case
		if (bf > 1 && balance_factor(root->left) < 0)
		{
			root->left = left_rotate(root->left);
			return right_rotate(root);
		}

		// Right Right Case
		if (bf < -1 && balance_factor(root->right) <= 0)
		{
			return left_rotate(root);
		}

		// Right Left Case
		if (bf < -1 && balance_factor(root->right) > 0)
		{
			root->right = right_rotate(root->right);
			return left_rotate(root);
		}

		return root;
	}

private:
	size_type size_{ 0 };
	link_type* root_{ nullptr };
	TCmp cmp_{};
};

}