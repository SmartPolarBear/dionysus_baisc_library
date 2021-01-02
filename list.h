#pragma once

#include <type_traits>

namespace kbl
{
	template<typename T>
	concept Pointer = std::is_pointer_v<T>;

	template<typename T>
	concept DoubleListNode =
	requires(T node)
	{
		node.get_next();
		node.get_prev();
		{ node.get_next() }->Pointer;
		{ node.get_prev() }->Pointer;
	};

	template<Pointer TPtr>
	class double_list_node
	{
	private:
		TPtr prev{ nullptr };
		TPtr next{ nullptr };
	public:
		TPtr get_prev() const
		{
			return prev;
		}

		void set_prev(TPtr prev)
		{
			double_list_node::prev = prev;
		}

		TPtr get_next() const
		{
			return next;
		}

		void set_next(TPtr next)
		{
			double_list_node::next = next;
		}


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
		using value_type = child_type;

		intrusive_double_list_iterator() = default;

		intrusive_double_list_iterator(const intrusive_double_list_iterator& other)
		{
			inner = other.inner;
		}

		intrusive_double_list_iterator& operator=(const intrusive_double_list_iterator& other)
		{
			inner = other.inner;
			return *this;
		}

		bool operator==(const intrusive_double_list_iterator& other) const
		{
			return inner == other.inner;
		}

		bool operator!=(const intrusive_double_list_iterator& other) const
		{
			return inner != other.inner;
		}

		constexpr explicit intrusive_double_list_iterator(child_ptr_type cp)
				: inner{ cp }
		{
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

		value_type& operator*()
		{
			return *inner;
		}

	private:
		child_ptr_type inner;
	};

	template<DoubleListNode TChild>
	class intrusive_double_list
	{
	public:
		using child_type = TChild;
		using child_ptr_type = child_type*;
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

		void remove(iterator_type iter)
		{
			list_remove(iter.inner);
		}

		void insert(TChild& child)
		{
			list_insert(&child, head.get_prev(), static_cast<child_type*>(&head));
		}

	private:
		double_list_node<child_ptr_type> head;

		void list_head_init(child_ptr_type head)
		{
			head->set_next(head);
			head->set_prev(head);
		}

		void list_insert(child_ptr_type newnode, child_ptr_type prev, child_ptr_type next)
		{
			prev->set_next(newnode);
			next->set_prev(newnode);

			newnode->set_next(next);
			newnode->set_prev(prev);
		}

		void list_remove(child_ptr_type prev, child_ptr_type next)
		{
			prev->set_next(next);
			next->set_prev(prev);
		}
	};
}