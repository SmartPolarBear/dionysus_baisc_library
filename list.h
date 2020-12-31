#pragma once

#include <type_traits>

namespace kbl
{
	template<typename T>
	concept Pointer = std::is_pointer_v<T>;

	template<typename T>
	concept DoubleListNode =
	Pointer<T> &&
	requires(T node)
	{
		node->next;
		node->prev;
		*node;
		{ node->next }->Pointer;
		{ node->prev }->Pointer;
	};

	template<Pointer TPtr>
	class double_list_node
	{
	protected:
		TPtr prev{ nullptr };
		TPtr next{ nullptr };
	};

	template<DoubleListNode TChildPtr>
	class intrusive_double_list_iterator
	{
	private:
		TChildPtr inner;

	public:
		using dummy_type = int;
		using value_type = decltype(*inner);

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

		constexpr explicit intrusive_double_list_iterator(TChildPtr cp)
				: inner{ cp }
		{
		}

		intrusive_double_list_iterator& operator++()
		{
			inner = inner->next;
			return *this;
		}

		intrusive_double_list_iterator& operator--()
		{
			inner = inner->prev;
			return *this;
		}

		intrusive_double_list_iterator operator++(dummy_type)
		{
			intrusive_double_list_iterator ret{ *this };
			++(*this);
			return ret;
		}

		intrusive_double_list_iterator operator--(dummy_type)
		{
			intrusive_double_list_iterator ret{ *this };
			--(*this);
			return ret;
		}

		value_type& operator*()
		{
			return *inner;
		}

	};

	template<DoubleListNode TChildPtr>
	class intrusive_double_list
	{
	public:


	protected:
		constexpr intrusive_double_list()
		{
			list_head_init(&head);
			list_insert(&sentinel, head, head);
		}

		

	private:
		void list_head_init(TChildPtr head)
		{
			head->next = head;
			head->prev = head;
		}

		void list_insert(TChildPtr newnode, TChildPtr prev, TChildPtr next)
		{
			prev->next = newnode;
			next->prev = newnode;

			newnode->next = next;
			newnode->prev = prev;
		}

		void list_remove(TChildPtr prev, TChildPtr next)
		{
			prev->next = next;
			next->prev = prev;
		}

		double_list_node<TChildPtr> head, sentinel;
	};
}