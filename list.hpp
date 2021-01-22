#pragma once

namespace kbl
{
// linked list head_
template<typename TParent>
struct list_head
{
	TParent* parent;
	bool is_head;
	list_head<TParent>* next, * prev;

	list_head() : parent{ nullptr }, is_head{ false }, next{ this }, prev{ this }
	{
	}

	explicit list_head(TParent* p) : parent{ p }, is_head{ false }, next{ this }, prev{ this }
	{
	}

	explicit list_head(TParent& p) : parent{ &p }, is_head{ false }, next{ this }, prev{ this }
	{
	}


	auto operator<=>(const list_head&) const = default;
};

template<typename T, bool Reverse = false, bool EnableLock = false>
class intrusive_list_iterator
{
public:

	template<typename S, list_head<S> S::*, bool E>
	friend
	class intrusive_list;

	using value_type = T;
	using head_type = list_head<value_type>;
	using size_type = size_t;
	using dummy_type = int;

public:
	intrusive_list_iterator() = default;

	explicit intrusive_list_iterator(head_type* h) : h_(h)
	{
	}

	intrusive_list_iterator(intrusive_list_iterator&& another) noexcept
	{
		h_ = another.h_;
		another.h_ = nullptr;
	}

	intrusive_list_iterator(const intrusive_list_iterator& another)
	{
		h_ = another.h_;
	}

	intrusive_list_iterator& operator=(const intrusive_list_iterator& another)
	{
		if (another.h_ == h_)return *this;

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

	bool operator==(intrusive_list_iterator const& other) const
	{
		return h_ == other.h_;
	}

	bool operator!=(intrusive_list_iterator const& other) const
	{
		return !(*this == other);
	}

	intrusive_list_iterator& operator++()
	{
		if constexpr (Reverse)
		{
			h_ = h_->prev;
		}
		else
		{
			h_ = h_->next;
		}
		return *this;
	}

	intrusive_list_iterator operator++(dummy_type) noexcept
	{
		intrusive_list_iterator rc(*this);
		operator++();
		return rc;
	}

	intrusive_list_iterator& operator--()
	{
		if constexpr (Reverse)
		{
			h_ = h_->next;
		}
		else
		{
			h_ = h_->prev;
		}
		return *this;
	}

	intrusive_list_iterator operator--(dummy_type) noexcept
	{
		intrusive_list_iterator rc(*this);
		operator--();
		return rc;
	}

private:
	head_type* h_;
};

#ifdef list_for
#error "list_for has been defined"
#endif

#ifdef list_for_safe
#error "list_for_safe has been defined"
#endif

#define list_for(pos, head) \
    for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

#define list_for_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

template<typename T, list_head<T> T::*Link, bool EnableLock = false>
class intrusive_list
{
public:
	using value_type = T;
	using head_type = list_head<value_type>;
	using size_type = size_t;
	using container_type = intrusive_list<T, Link, EnableLock>;
	using iterator_type = intrusive_list_iterator<T, false, EnableLock>;
	using riterator_type = intrusive_list_iterator<T, true, EnableLock>;
	using const_iterator_type = const iterator_type;

public:
	/// New empty list
	constexpr intrusive_list()
	{
		list_init(&head_);
	}

	/// Isn't copiable
	intrusive_list(const intrusive_list&) = delete;

	intrusive_list& operator=(const intrusive_list&) = delete;

	/// Move constructor
	intrusive_list(intrusive_list&& another) noexcept
	{
		list_init(&head_);

		head_type* iter = nullptr;
		head_type* t = nullptr;

		list_for_safe(iter, t, &another.head_)
		{
			list_remove(iter);
			list_add(iter, this->head_);
		}

		size_ = another.size_;
		another.size_ = 0;
	}

	/// First element
	/// \return the reference to first element
	T& front()
	{
		return *head_.next->parent;
	}

	/// Last element
	/// \return the reference to first element
	T& back()
	{
		return *head_.prev->parent;
	}

	iterator_type begin()
	{
		return iterator_type{ head_.next };
	}

	iterator_type end()
	{
		return iterator_type{ &head_ };
	}

	const_iterator_type cbegin()
	{
		return const_iterator_type{ head_.next };
	}

	const_iterator_type cend()
	{
		return const_iterator_type{ &head_ };
	}

	riterator_type rbegin()
	{
		return riterator_type{ head_.prev };
	}

	riterator_type rend()
	{
		return riterator_type{ &head_ };
	}

	void insert(const_iterator_type iter, T& item)
	{
		list_add(&item.*Link, iter.h_);
	}

	void insert(const_iterator_type iter, T* item)
	{
		list_add(&(item->*Link), iter.h_);
	}

	void insert(riterator_type iter, T& item)
	{
		list_add(&item.*Link, iter.h_);
	}

	void insert(riterator_type iter, T* item)
	{
		list_add(&(item->*Link), iter.h_);
	}

	void erase(iterator_type it)
	{
		list_remove(&it.*Link);
	}

	void erase(riterator_type it)
	{
		list_remove(&it.*Link);
	}

	/// Remove item by value. **it takes liner time**
	/// \param val
	void remove(T& val)
	{
		head_type* it = nullptr, t = nullptr;
		list_for_safe(it, t, &head_)
		{
			if (*it->parent == val)
			{
				list_remove(it);
			}
		}
	}

	void pop_back()
	{
		list_remove(head_.prev);
	}

	void push_back(T& item)
	{
		list_add_tail(&item.*Link, &head_);
	}

	void push_back(T* item)
	{
		list_add_tail(&(item->*Link), &head_);
	}

	void pop_front()
	{
		list_remove(head_.next);
	}

	void push_front(T& item)
	{
		list_add(&item.*Link, &head_);
	}

	void push_front(T* item)
	{
		list_add(&(item->*Link), &head_);
	}

	void clear()
	{
		head_type* iter = nullptr, * t = nullptr;
		list_for_safe(iter, t, &head_)
		{
			list_remove(iter);
		}
	}

	/// swap this and another
	/// \param another
	void swap(container_type& another) noexcept
	{
		list_swap(&head_, &another.head_);
	}

	/// Merge two **sorted** list, after that another becomes empty
	/// \param another
	void merge(container_type& another)
	{
		merge(another, [](const T& a, const T& b)
		{
			return a < b;
		});
	}

	/// Merge two **sorted** list, after that another becomes empty
	/// \tparam Compare cmp(a,b) returns true if a comes before b
	/// \param another
	/// \param cmp
	template<typename Compare>
	void merge(container_type& another, Compare cmp)
	{
		if (another.head_ == head_)return;

		head_type t_head{ nullptr };
		head_type* i1 = head_.next, * i2 = another.head_.next;
		while (i1 != &head_ && i2 != &another.head_)
		{
			if (cmp(*(i1->parent), *(i2->parent)))
			{
				auto next = i1->next;
				list_remove(i1);
				list_add_tail(i1, &t_head);
				i1 = next;
			}
			else
			{
				auto next = i2->next;
				list_remove(i2);
				list_add_tail(i2, &t_head);
				i2 = next;
			}
		}

		while (i1 != &head_)
		{
			auto next = i1->next;
			list_remove_init(i1);
			list_add_tail(i1, &t_head);
			i1 = next;
		}

		while (i2 != &another.head_)
		{
			auto next = i2->next;
			list_remove_init(i2);
			list_add_tail(i2, &t_head);
			i2 = next;
		}

		list_swap(&head_, &t_head);
	}

	/// join two lists
	/// \param other
	void splice(intrusive_list& other)
	{
		list_splice(&other.head_, &head_);
	}


	/// Join two lists, insert other 's item after the pos
	/// \param pos
	/// \param other
	void splice(const_iterator_type pos, intrusive_list& other)
	{
		list_splice(&other.head_, pos.h_);
	}

	/// Join two lists, insert other 's item after the pos
	/// \param pos
	/// \param other
	void splice(riterator_type pos, intrusive_list& other)
	{
		list_splice(&other.head_, pos.h_);
	}

	[[nodiscard]] size_type size() const
	{
		return size_;
	}

	[[nodiscard]] size_type size_slow() const
	{
		size_type sz = 0;
		head_type* iter = nullptr;
		list_for(iter, &head_)
		{
			sz++;
		}
		return sz;
	}

	[[nodiscard]] bool empty() const
	{
		return list_empty(&head_);
	}

private:
	template<typename TParent>
	static inline void util_list_init(list_head<TParent>* head)
	{
		head->next = head;
		head->prev = head;
	}

	template<typename TParent>
	static inline void
	util_list_add(list_head<TParent>* newnode, list_head<TParent>* prev, list_head<TParent>* next)
	{
		prev->next = newnode;
		next->prev = newnode;

		newnode->next = next;
		newnode->prev = prev;
	}

	template<typename TParent>
	static inline void util_list_remove(list_head<TParent>* prev, list_head<TParent>* next)
	{
		prev->next = next;
		next->prev = prev;
	}

	template<typename TParent>
	static inline void util_list_remove_entry(list_head<TParent>* entry)
	{
		util_list_remove(entry->prev, entry->next);
	}

	template<typename TParent>
	static inline void
	util_list_splice(const list_head<TParent>* list, list_head<TParent>* prev, list_head<TParent>* next)
	{
		list_head<TParent>* first = list->next, * last = list->prev;

		first->prev = prev;
		prev->next = first;

		last->next = next;
		next->prev = last;
	}

	// initialize the list
	template<typename TParent>
	static inline void list_init(list_head<TParent>* head)
	{
		util_list_init(head);
	}

// add the new node after the specified head_
	template<typename TParent>
	static inline void list_add(list_head<TParent>* newnode, list_head<TParent>* head)
	{
		util_list_add(newnode, head, head->next);
	}

// add the new node before the specified head_
	template<typename TParent>
	static inline void list_add_tail(list_head<TParent>* newnode, list_head<TParent>* head)
	{
		util_list_add(newnode, head->prev, head);
	}

// delete the entry
	template<typename TParent>
	static inline void list_remove(list_head<TParent>* entry)
	{
		util_list_remove_entry(entry);

		entry->prev = nullptr;
		entry->next = nullptr;
	}

	template<typename TParent>
	static inline void list_remove_init(list_head<TParent>* entry)
	{
		util_list_remove_entry(entry);

		entry->prev = nullptr;
		entry->next = nullptr;

		list_init(entry);
	}

// replace the old entry with newnode
	template<typename TParent>
	static inline void list_replace(list_head<TParent>* old, list_head<TParent>* newnode)
	{
		newnode->next = old->next;
		newnode->next->prev = newnode;
		newnode->prev = old->prev;
		newnode->prev->next = newnode;
	}

	template<typename TParent>
	static inline bool list_empty(const list_head<TParent>* head)
	{
		return (head->next) == head;
	}


	template<typename TParent>
	static inline void list_swap(list_head<TParent>* e1, list_head<TParent>* e2)
	{
		auto* pos = e2->prev;
		list_remove(e2);
		list_replace(e1, e2);

		if (pos == e1)
		{
			pos = e2;
		}

		list_add(e1, pos);
	}


	/// \tparam TParent
	/// \param list	the new list to add
	/// \param head the place to add it in the list
	template<typename TParent>
	static inline void list_splice(const list_head<TParent>* list, list_head<TParent>* head)
	{
		if (!list_empty(list))
		{
			util_list_splice(list, head, head->next);
		}
	}


	head_type head_{ nullptr };
	size_type size_{ 0 };
};


#undef list_for
#undef list_for_safe

} // namespace

