#pragma once

#include <type_traits>

namespace kbl
{
	template<typename T>
	concept Pointer = std::is_pointer_v<T>;

	template<typename TFrom, typename TTo>
	concept ConvertibleTo = std::is_convertible_v<TFrom, TTo>;

	template<typename TChild>
	class double_list_node
	{
	private:
		using node_ptr_type = double_list_node*;
		using child_type = TChild;
		using child_ptr_type = child_type*;

		node_ptr_type prev{ nullptr };
		node_ptr_type next{ nullptr };
	public:
		[[nodiscard]]node_ptr_type get_prev() const
		{
			return prev;
		}

		void set_prev(node_ptr_type _prev)
		{
			prev = _prev;
		}

		[[nodiscard]]node_ptr_type get_next() const
		{
			return next;
		}

		void set_next(node_ptr_type _next)
		{
			next = _next;
		}

		void remove_self() noexcept
		{
			this->next->prev = this->prev;
			this->prev->next = this->next;

			this->next = nullptr;
			this->prev = nullptr;
		}
	};

	template<typename T>
	concept DoubleListNode =
	ConvertibleTo<T, double_list_node<T>> &&
	requires(T node)
	{
		node.get_next();
		node.get_prev();
		node.remove_self();
		{ node.get_next() }->Pointer;
		{ node.get_prev() }->Pointer;
	};

	template<DoubleListNode TChild>
	class intrusive_double_list_iterator
	{
	public:
		template<DoubleListNode>
		friend
		class intrusive_double_list;

		using dummy_type = int;

		using child_type = TChild;
		using child_ptr_type = child_type*;
		using node_type = double_list_node<TChild>;
		using node_ptr_type = node_type*;

		intrusive_double_list_iterator& operator=(const intrusive_double_list_iterator& other)
		{
			inner = other.inner;
			return *this;
		}

		child_ptr_type operator->()
		{
			return static_cast<child_ptr_type >(inner);
		}

		bool operator==(const intrusive_double_list_iterator& other) const
		{
			return inner == other.inner;
		}

		bool operator!=(const intrusive_double_list_iterator& other) const
		{
			return inner != other.inner;
		}

		intrusive_double_list_iterator& operator++()
		{

			inner = inner->get_next();
			return *this;
		}

		intrusive_double_list_iterator& operator--()
		{
			inner = inner->get_prev();
			return *this;
		}

		const intrusive_double_list_iterator operator++(dummy_type)
		{
			intrusive_double_list_iterator ret{ *this };
			++(*this);
			return ret;
		}

		const intrusive_double_list_iterator operator--(dummy_type)
		{
			intrusive_double_list_iterator ret{ *this };
			--(*this);
			return ret;
		}

		child_type& operator*()
		{
			return static_cast<child_type&>(*inner);
		}

	protected:
		intrusive_double_list_iterator() noexcept = default;

		intrusive_double_list_iterator(const intrusive_double_list_iterator& other) noexcept
		{
			inner = other.inner;
		}

		intrusive_double_list_iterator(intrusive_double_list_iterator&& other) noexcept
		{
			inner = other.inner;
			other.inner = nullptr;
		}

		constexpr explicit intrusive_double_list_iterator(child_ptr_type cp)
				: inner{ static_cast<node_ptr_type>(cp) }
		{
		}

		constexpr explicit intrusive_double_list_iterator(node_ptr_type cp)
				: inner{ cp }
		{
		}

	private:
		node_ptr_type inner;
	};

	template<DoubleListNode TChild>
	class intrusive_double_list
	{
	public:
		using size_type = size_t;
		using child_type = TChild;
		using child_ptr_type = child_type*;
		using node_type = double_list_node<TChild>;
		using node_ptr_type = node_type*;
		using iterator_type = intrusive_double_list_iterator<child_type>;
		using const_iterator_type = const intrusive_double_list_iterator<child_type>;

		constexpr intrusive_double_list()
		{
			list_head_init(static_cast<child_type*>(&head));
		}

		iterator_type begin()
		{
			return iterator_type{ head.get_next() };
		}

		const_iterator_type& cbegin()
		{
			return const_iterator_type{ head.get_next() };
		}

		iterator_type end()
		{
			return iterator_type{ static_cast<child_type*>(&head) };
		}

		const_iterator_type cend()
		{
			return iterator_type{ static_cast<child_type*>(&head) };
		}

		/// Remove the node pointed by iterator
		/// \param iter
		void remove(iterator_type iter)
		{
			list_remove(iter.inner);
		}

		/// Remove the child
		/// \param c the child to remove
		void remove(child_type& c)
		{
			c.remove_self();
		}

		/// Insert before head
		/// \param child the child to insert
		void insert(TChild& child)
		{
			list_insert(&child, head.get_prev(), static_cast<child_type*>(&head));
		}

		/// Insert after
		/// \param iter insert after this iterator
		/// \param child the child to insert
		void insert(iterator_type iter, TChild& child)
		{
			list_insert(&child, &(*iter), iter->get_next());
		}

		void clear() noexcept
		{

		}

		[[nodiscard]]size_type size() const noexcept
		{

		}



	private:
		node_type head;

		void list_head_init(node_ptr_type head)
		{
			head->set_next(head);
			head->set_prev(head);
		}

		void list_insert(node_ptr_type newnode, node_ptr_type prev, node_ptr_type next)
		{
			prev->set_next(newnode);
			next->set_prev(newnode);

			newnode->set_next(next);
			newnode->set_prev(prev);
		}

		void list_remove(node_ptr_type prev, node_ptr_type next)
		{
			prev->set_next(next);
			next->set_prev(prev);
		}
	};
}