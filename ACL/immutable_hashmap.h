#pragma once
#include <memory>
#include <stack>
#include <../immutable_list.h>

namespace Lib {
	template<typename K, typename V>
	class ImmutableHashMap {
		struct Node {
			int max_hash;
			Node(const int max_hash) : max_hash{ max_hash } {};
			virtual std::pair<std::shared_ptr<Node>, bool> inserted_or_updated(const int hash, const std::pair<K, V>& key_value) const = 0;
			virtual std::shared_ptr<Node> inserted_if_absence(const int hash, const std::pair<K, V>& key_value) const = 0;
			virtual std::pair<std::shared_ptr<Node>, bool> erased(const int hash, const K& key) const = 0;
			virtual std::pair<const std::shared_ptr<std::pair<K, V>>, std::shared_ptr<Node>> get_or_put_default(const int hash, const K& key) const = 0;
			virtual std::pair<const std::shared_ptr<std::pair<K, V>>, std::shared_ptr<Node>> get_or_put(const int hash, const K& key, const V& value) const = 0;
			virtual const std::shared_ptr<std::pair<K, V>> get_or_null(const int hash, const K& key) const = 0;
		};
		struct Leaf : public Node {
			ImmutableListMap<K, V> list;
			Leaf(const int max_hash, ImmutableListMap<K, V>&& list) : Node{ max_hash }, list{ std::move(list) }{};
			std::pair<std::shared_ptr<Node>, bool> inserted_or_updated(const int hash, const std::pair<K, V>& key_value) const override;
			std::shared_ptr<Node> inserted_if_absence(const int hash, const std::pair<K, V>& key_value) const override;
			std::pair<std::shared_ptr<Node>, bool> erased(const int hash, const K& key) const override;
		};
		struct Branch : public Node {
			int branch_bit() const;
			std::shared_ptr<Node> left, right;
			Branch(const int max_hash, const std::shared_ptr<Node>& left, const std::shared_ptr<Node>& right) : Node{ max_hash }, left{ left }, right{ right }{};
			Branch(const int max_hash, std::shared_ptr<Node>&& left, const std::shared_ptr<Node>& right) : Node{ max_hash }, left{ std::move(left) }, right{ right }{};
			Branch(const int max_hash, const std::shared_ptr<Node>& left, std::shared_ptr<Node>&& right) : Node{ max_hash }, left{ left }, right{ std::move(right) }{};
			std::pair<std::shared_ptr<Node>, bool> inserted_or_updated(const int hash, const std::pair<K, V>& key_value) const override;
			std::shared_ptr<Node> inserted_if_absence(const int hash, const std::pair<K, V>& key_value) const override;
			std::pair<std::shared_ptr<Node>, bool> erased(const int hash, const K& key) const override;
		};
		std::shared_ptr<Node> root;
		int size;
	public:
		struct Iter {
			std::stack<Node*> stack;
			bool operator==(const Iter& other) const;
			bool operator!=(const Iter& other) const;
			std::pair<K, V>* operator->() const;
			const std::pair<K, V>& operator*() const;
			Iter operator++();
		};
		Iter begin() const;
		Iter end() const;
		Iter find(const K& key) const;
		ImmutableHashMap<K, V> inserted(const std::pair<K, V>& key_value) const;
		ImmutableHashMap<K, V> inserted(const K& key, const V& value) const;
		ImmutableHashMap<K, V> removed(const K& key) const;
		const V& get(const K& key) const;
		std::pair<const V&, ImmutableHashMap<K, V>> get_or_put_default(const K& key) const;
		std::pair<const V&, ImmutableHashMap<K, V>> get_or_put(const K& key, const V& value) const;
	};
}