#pragma once
#include <utility>
#include <memory>

namespace Lib {
	template<typename K, typename V>
	struct ImmutableListMap {
		struct Node {
			std::shared_ptr<std::pair<K, V>> head;
			std::shared_ptr<Node> tail;
			Node(const std::shared_ptr<std::pair<K, V>>& head, const std::shared_ptr<Node>& tail) : head{ head }, tail{ tail }{};
			Node(std::shared_ptr<std::pair<K, V>>&& head, const std::shared_ptr<Node>& tail) : head{ std::move(head) }, tail{ tail }{};

		};
		struct ConstNodeIter {
			Node* current;
			const std::pair<K, V>& operator*();
			const std::pair<K, V>* operator->() const;
			ConstNodeIter operator++();
			bool operator==(const ConstNodeIter& other) const;
			bool operator!=(const ConstNodeIter& other) const;
		};
		std::shared_ptr<Node> node;
		ImmutableListMap<K, V> append_head(const std::pair<K, V>& key_value) const;
		ImmutableListMap<K, V> append_head(std::pair<K, V>&& key_value) const;
		ImmutableListMap<K, V> tail() const;
		ConstNodeIter find(const K& key) const;
		std::pair<ConstNodeIter, ImmutableListMap<K, V>> new_node_if_absence(const K& key) const;
		ConstNodeIter begin() const;
		ConstNodeIter end() const;
		ConstNodeIter cbegin() const;
		ConstNodeIter cend() const;
		bool empty() const;
	};

	template<typename K, typename V>
	typename ImmutableListMap<K, V>::ConstNodeIter ImmutableListMap<K, V>::begin() const
	{
		return ConstNodeIter{ node.get() };
	}

	template<typename K, typename V>
	typename ImmutableListMap<K, V>::ConstNodeIter ImmutableListMap<K, V>::end() const
	{
		return ConstNodeIter{ nullptr };
	}

	template<typename K, typename V>
	typename ImmutableListMap<K, V>::ConstNodeIter ImmutableListMap<K, V>::cbegin() const
	{
		return ConstNodeIter{ node.get() };
	}

	template<typename K, typename V>
	typename ImmutableListMap<K, V>::ConstNodeIter ImmutableListMap<K, V>::cend() const
	{
		return ConstNodeIter{ nullptr };
	}

	template<typename K, typename V>
	bool ImmutableListMap<K, V>::empty() const
	{
		return node == nullptr;
	}

	template<typename K, typename V>
	const std::pair<K, V>& ImmutableListMap<K, V>::ConstNodeIter::operator*()
	{
		return *current->head;
	}

	template<typename K, typename V>
	const std::pair<K, V>* ImmutableListMap<K, V>::ConstNodeIter::operator->() const
	{
		return current->head.get();
	}

	template<typename K, typename V>
	typename ImmutableListMap<K, V>::ConstNodeIter ImmutableListMap<K, V>::ConstNodeIter::operator++()
	{
		current = current->tail.get();
		return *this;
	}

	template<typename K, typename V>
	bool ImmutableListMap<K, V>::ConstNodeIter::operator==(const ConstNodeIter& other) const
	{
		return current == other.current;
	}

	template<typename K, typename V>
	bool ImmutableListMap<K, V>::ConstNodeIter::operator!=(const ConstNodeIter& other) const
	{
		return current != other.current;
	}
}