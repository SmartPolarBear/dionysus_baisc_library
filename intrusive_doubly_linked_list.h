#pragma once

#include <type_traits>

namespace kbl
{
	template<typename T>
	concept Pointer = std::is_pointer_v<T>;

	template<typename TFrom, typename TTo>
	concept ConvertibleTo = std::is_convertible_v<TFrom, TTo>;

	template<typename T>
	requires (!Pointer<T>)
	class doubly_linked_node_state
	{
		template<typename S, doubly_linked_node_state<S> S::*>
		friend
		class intrusive_doubly_linked_list;

		template<typename S, doubly_linked_node_state<S> S::*>
		friend
		class intrusive_doubly_linked_list_iterator;

		T* next{ nullptr };
		doubly_linked_node_state<T>* prev{ nullptr };

	public:
		doubly_linked_node_state() = default;

		doubly_linked_node_state(const doubly_linked_node_state&) = delete;

		void operator=(const doubly_linked_node_state&) = delete;
	};

	template<typename T, doubly_linked_node_state<T> T::*NS>
	class intrusive_doubly_linked_list_iterator
	{

	public:
		explicit intrusive_doubly_linked_list_iterator(doubly_linked_node_state<T>* ns) : node_state(ns)
		{
		}

		T& operator*()
		{
			return *this->operator->();
		}

		T* operator->()
		{
			return this->node_state->next;
		}

		bool operator==(intrusive_doubly_linked_list_iterator const& other) const
		{
			return this->node_state == other.node_state;
		}

		bool operator!=(intrusive_doubly_linked_list_iterator const& other) const
		{
			return !(*this == other);
		}

		intrusive_doubly_linked_list_iterator& operator++()
		{
			this->node_state = &(this->node_state->next->*NS);
			return *this;
		}

		intrusive_doubly_linked_list_iterator operator++(int)
		{
			intrusive_doubly_linked_list_iterator rc(*this);
			this->operator++();
			return rc;
		}

		intrusive_doubly_linked_list_iterator& operator--()
		{
			this->node_state = this->node_state->prev;
			return *this;
		}

		intrusive_doubly_linked_list_iterator operator--(int)
		{
			intrusive_doubly_linked_list_iterator rc(*this);
			this->operator--();
			return rc;
		}

	private:
		template<typename S, doubly_linked_node_state<S> S::*>
		friend
		class intrusive_doubly_linked_list;

		doubly_linked_node_state<T>* node_state;
	};

	template<typename T, doubly_linked_node_state<T> T::*NS>
	class intrusive_doubly_linked_list
	{
	public:
		intrusive_doubly_linked_list()
		{
			this->head.prev = &this->head;
		}

		intrusive_doubly_linked_list_iterator<T, NS> begin()
		{
			return intrusive_doubly_linked_list_iterator<T, NS>(&this->head);
		}

		intrusive_doubly_linked_list_iterator<T, NS> end()
		{
			return intrusive_doubly_linked_list_iterator<T, NS>(this->head.prev);
		}

		T& front()
		{
			return *this->head.next;
		}

		T& back()
		{
			return *(this->head.prev->prev->next);
		}

		bool empty() const
		{
			return &this->head == this->head.prev;
		}

		void push_back(T& node)
		{
			this->insert(this->end(), node);
		}

		void push_front(T& node)
		{
			this->insert(this->begin(), node);
		}

		void insert(intrusive_doubly_linked_list_iterator<T, NS> pos, T& node)
		{
			(node.*NS).next = pos.node_state->next;
			((node.*NS).next
			 ? (pos.node_state->next->*NS).prev
			 : this->head.prev) = &(node.*NS);
			(node.*NS).prev = pos.node_state;
			pos.node_state->next = &node;
		}

		intrusive_doubly_linked_list_iterator<T, NS> erase(intrusive_doubly_linked_list_iterator<T, NS> it)
		{
			it.node_state->next = (it.node_state->next->*NS).next;
			(it.node_state->next
			 ? (it.node_state->next->*NS).prev
			 : this->head.prev) = it.node_state;
			return it;
		}

	private:
		doubly_linked_node_state<T> head;
	};
}