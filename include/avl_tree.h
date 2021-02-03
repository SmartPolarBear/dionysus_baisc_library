#pragma once

#include <utility.h>

#include <compare>

namespace kbl
{

template<typename T>
concept AVLTreeKey=
requires(T a, T b)
{
	a <=> b;
};

template<AVLTreeKey T>
struct avl_default_comparer
{
	auto operator()(const T& a, const T& b)
	{
		return a <=> b;
	}
};

template<typename T>
struct avl_default_deleter
{
	void operator()(T* ptr)
	{
		delete ptr;
	}
};

template<typename TOwner, auto TOwner::*Key>
struct avl_tree_link
{
	TOwner* owner{ nullptr };

	size_t height{ 1 };

	bool sentinel{ false };

	avl_tree_link* left{ nullptr }, * right{ nullptr };

	union
	{
		avl_tree_link* parent;
		avl_tree_link* root; // used when sentinel == true
	};

	void set_left(avl_tree_link* l)
	{
		if (left && left->parent == this)
			left->parent = nullptr;

		left = l;

		if (left)
			left->parent = this;
	}

	void set_right(avl_tree_link* r)
	{
		if (right && right->parent == this)
			right->parent = nullptr;

		right = r;

		if (right)
			right->parent = this;
	}

	[[nodiscard]] avl_tree_link()
			: owner(nullptr),
			  height(1),
			  sentinel(false),
			  left(nullptr),
			  right(nullptr),
			  parent(nullptr)
	{

	}

	[[nodiscard]] explicit avl_tree_link(TOwner* own)
			: owner(own),
			  height(1),
			  sentinel(false),
			  left(nullptr),
			  right(nullptr),
			  parent(nullptr)
	{

	}


	[[nodiscard]] explicit avl_tree_link(bool is_sentinel)
			: owner(nullptr),
			  height(1),
			  sentinel(is_sentinel),
			  left(nullptr),
			  right(nullptr),
			  parent(nullptr)
	{

	}
};

template<typename T,
		typename TContainer,
		bool EnableLock = false>
class avl_tree_iterator
{
public:

	template<typename S, AVLTreeKey SK,
			SK S::*K,
			avl_tree_link<S, K> T::*L,
			bool LK = false,
			bool D = false,
			typename TC=avl_default_comparer<S>,
			typename TD=avl_default_deleter<S>>
	class avl_tree;

	using value_type = T;
	using size_type = size_t;
	using ssize_type = int64_t;
	using container_type = TContainer;
	using link_type = typename container_type::link_type;

	using dummy_type = int;


public:
	avl_tree_iterator() = default;

	explicit avl_tree_iterator(link_type* h, container_type* cont) :
			h_(h),
			cont_(cont)
	{
	}

	avl_tree_iterator(avl_tree_iterator&& another) noexcept
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
		return h_->owner;
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
		h_ = cont_->next_of(h_);
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
		h_ = cont_->prev_of(h_);

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
	container_type* cont_;
};


template<typename T, AVLTreeKey TKey,
		TKey T::*Key,
		avl_tree_link<T, Key> T::*Link,
		bool EnableLock = false,
		bool CallDeleteOnRemoval = false,
		typename TCmp= avl_default_comparer<TKey>,
		typename TDeleter=avl_default_deleter<T>>
class avl_tree
{
public:
	using value_type = T;
	using size_type = size_t;
	using ssize_type = int64_t;
	using link_type = avl_tree_link<T, Key>;
	using iterator_type = avl_tree_iterator<T, avl_tree, EnableLock>;
	using riterator_type = kbl::reversed_iterator<iterator_type>;

	template<typename, typename, bool>
	friend
	class avl_tree_iterator;

public:
	avl_tree()
	{
	}

	void insert(T* val)
	{
		root_ = insert(&(val->*Link), root_);
		update_sentinels();

	}

	void insert(T& val)
	{
		insert(&val);
	}

	void remove(T* val)
	{
		root_ = remove(&(val->*Link), root_);
		update_sentinels();

		if constexpr (CallDeleteOnRemoval)
		{
			deleter_(val);
		}
	}

	void remove(T& val)
	{
		remove(&val);
	}

	void clear()
	{
		for (link_type* victim = max_node(root_); victim; victim = max_node(root_))
		{
			remove(victim->owner);
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
		return iterator_type{ first_of(root_), this };
	}

	iterator_type end()
	{
		return iterator_type{ &back_sentinel_, this };
	}

	riterator_type rbegin()
	{
		return riterator_type{ last_of(root_), this };
	}

	riterator_type rend()
	{
		return riterator_type{ &front_sentinel_, this };
	}

	T& front()
	{
		return *(first_of(root_)->owner);
	}

	T& back()
	{
		return *(last_of(root_)->owner);
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

		root->set_right(right->left);
		right->set_left(root);
//		root->right = right->left;
//		right->left = root;

		update_height(right);
		update_height(root);

		return right;
	}

	static inline link_type* right_rotate(link_type* root)
	{
		auto left = root->left;

		root->set_left(left->right);
		left->set_right(root);

//		root->left = left->right;
//		left->right = root;

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

	void update_sentinels()
	{
		back_sentinel_.root = root_;
		front_sentinel_.root = root_;
	}

	link_type* next_of(link_type* node)
	{
		if (node->sentinel)return node;

		if (node->parent == nullptr && node->left == nullptr && node->right == nullptr)
			return nullptr;

		if (node->right)
		{
			return first_of(node->right);
		}

		link_type* parent = nullptr;
		while ((parent = node->parent) && node == parent->right)
			node = parent;

		if (!parent)return &back_sentinel_;

		return parent;
	}

	link_type* prev_of(link_type* node)
	{
		if (node->sentinel)return last_of(node->root);

		if (node->parent == nullptr && node->left == nullptr && node->right == nullptr)return nullptr;

		if (node->left)
		{
			return last_of(node->left);
		}

		link_type* parent = nullptr;
		while ((parent = node->parent) && node == parent->left)
			node = parent;

		if (!parent)return &front_sentinel_; // the very first

		return parent;
	}

	static inline link_type* first_of(link_type* root)
	{
		if (root->sentinel)return nullptr;

		if (root->left == nullptr)
			return root;

		auto left = root->left;
		while (left->left && !left->left->sentinel)
			left = left->left;

		return left;
	}

	static inline link_type* last_of(link_type* root)
	{
		if (root->sentinel)return nullptr;

		if (root->right == nullptr)
			return root;

		auto right = root->right;
		while (right->right && !right->right->sentinel)
			right = right->right;

		return right;
	}

	std::strong_ordering compare(link_type* n1, link_type* n2)
	{
		if (n1->sentinel)return 1 <=> -1;
		else if (n2->sentinel)return -1 <=> 1;
		else return cmp_(key_of(n1), key_of(n2));
	}

	link_type* insert(link_type* newnode, link_type* root)
	{
		if (root == nullptr)
		{
			++size_;
			return newnode;
		}

		auto cmp_val = compare(newnode, root);
		if (cmp_val < 0)
		{
//			auto new_left = insert(newnode, root->left);
//			root->left = new_left;
//			new_left->parent = root;
			root->set_left(insert(newnode, root->left));
		}
		else if (cmp_val > 0)
		{
//			auto new_right = insert(newnode, root->right);
//			root->right = new_right;
//			new_right->parent = root;
			root->set_right(insert(newnode, root->right));
		}
		else
		{
			return root;
		}

		update_height(root);

		auto bf = balance_factor(root);
		if (bf > 1 && compare(newnode, root->left) > 0)
		{
//			root->left = left_rotate(root->left);
			root->set_left(left_rotate(root->left));
			return right_rotate(root);
		}
		else if (bf > 1 && compare(newnode, root->left) < 0)
		{
			return right_rotate(root);
		}
		else if (bf < -1 && compare(newnode, root->right) > 0)
		{
			return left_rotate(root);
		}
		else if (bf < -1 && compare(newnode, root->right) < 0)
		{
//			root->right = right_rotate(root->right);
			root->set_right(right_rotate(root->right));
			return left_rotate(root);
		}

		return root;
	}

	link_type* do_remove(link_type* node, link_type* root)
	{
		if (root->left == nullptr && root->right == nullptr)
		{
			root->parent = nullptr;
			root = nullptr;
			--size_;
		}
		else if (root->left == nullptr || root->right == nullptr)
		{
			auto newroot = root->left ? root->left : root->right;
			newroot->parent = root->parent;
			root->parent = nullptr;
			root = newroot;
			--size_;
		}
		else
		{
			auto newroot = min_node(root->right);

			// it must be a leaf, so this will not cause infinite recursion
//			root->right = remove(root->right, newroot);
			root->set_right(remove(root->right, newroot));

//			newroot->left = root->left;
//			newroot->right = root->right;
//			newroot->parent = root->parent;

			newroot->set_left(root->left);
			newroot->set_right(root->right);

			newroot->parent = root->parent;
			root->parent = nullptr;

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
//			root->left = remove(node, root->left);
			root->set_left(remove(node, root->left));
		}
		else if (cmp_val > 0)
		{
//			root->right = remove(node, root->right);
			root->set_right(remove(node, root->right));
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
//			root->left = left_rotate(root->left);
			root->set_left(left_rotate(root->left));
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
//			root->right = right_rotate(root->right);
			root->set_right(right_rotate(root->right));
			return left_rotate(root);
		}

		return root;
	}


private:
	size_type size_{ 0 };
	link_type* root_{ nullptr };
	TCmp cmp_{};
	TDeleter deleter_{};

	link_type back_sentinel_{ true };
	link_type front_sentinel_{ true };
};

}