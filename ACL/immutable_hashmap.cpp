#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
#include <iomanip>
#include <queue>
#include <string>
#include <map>
#include <fstream>
#include <cassert>
#include <stack>
#include <climits>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>
#include <cfloat>
#include <numeric>
#include <random>
#include <sstream>
#include <bitset>
#include <complex>
#include <chrono>



namespace {
template<typename K, typename V>
class ListMap {
public:
	struct Node {
		std::shared_ptr<std::pair<K, V>> head;
		std::shared_ptr<Node> tail;
		Node(std::pair<K, V>&& pair) : head{ std::make_shared<std::pair<K, V>>(std::move(pair)) }, tail{ nullptr }{};
		Node(std::pair<K, V>&& pair, const std::shared_ptr<Node>& tail) : head{ std::make_shared<std::pair<K, V>>(std::move(pair)) }, tail{ tail }{};
		template<typename M>
		Node(const K& key, M&& obj, const std::shared_ptr<Node>& tail) : head{ std::make_shared<std::pair<K, V>>(key, std::forward<M>(obj)) }, tail{ tail }{};
		template<typename M>
		Node(K&& key, M&& obj, const std::shared_ptr<Node>& tail) : head{ std::make_shared<std::pair<K, V>>(std::forward<K>(key), std::forward<M>(obj)) }, tail{ tail } {}
		Node(const std::shared_ptr<std::pair<K, V>>& head, const std::shared_ptr<Node>& tail) : head{ head }, tail{ tail } {}
		Node(std::shared_ptr<std::pair<K, V>>&& head, const std::shared_ptr<Node>& tail) : head{ std::move(head) }, tail{ tail } {}
	};
	struct Iter {
		Node* current{ nullptr };
		bool operator==(const Iter& other) const;
		bool operator!=(const Iter& other) const;
		Iter& operator++();
		const std::pair<K, V>* operator->() const;
		const std::pair<K, V>& operator*() const;
		const std::pair<K, V>* get() const;
		bool is_end() const;
		void skip_to_end();
	};
	std::shared_ptr<Node> list{ nullptr };
	ListMap() {};
	ListMap(std::pair<K, V>&& pair) : list{ std::make_shared<Node>(std::move(pair)) } {};
	template<typename M>
	ListMap(K&& key, M&& obj) : list{ std::make_shared<Node>(std::make_pair(std::forward<K>(key), std::forward<M>(obj))), nullptr } {};
	template<typename M>
	ListMap(const K& key, M&& obj) : list{ std::make_shared<Node>(std::make_pair(key, std::forward<M>(obj))) , nullptr } {};
	Iter begin() const;
	Iter end() const;
	const std::pair<K, V>& head() const;
	template<typename M>
	ListMap<K, V> appended(K&& key, M&& obj) const;
	template<typename M>
	ListMap<K, V> appended(const K& key, M&& obj) const;
	ListMap<K, V> appended(std::pair<K, V>&& pair) const;
	ListMap<K, V> appended(const std::pair<K, V>& pair) const;
	Iter find_first(const K& key) const;
	std::pair<Iter, ListMap<K, V>> erased_first(const K& key) const;
	bool empty() const;
private:
	ListMap(std::shared_ptr<Node>&& list) : list{ std::move(list) } {};
};

template<typename K, typename V, typename Hash = std::hash<K>>
class HashMap {
	using HashVal = decltype(Hash()(std::declval<K>()));

	struct Leaf;
	struct Branch;
	struct Node {
		HashVal right_hash;
		Node(const HashVal hash) : right_hash(hash) {};
		static inline HashVal right_bit(const HashVal hash);
		// nullptr => not found
		virtual const Leaf* find_or_null(const HashVal hash) const = 0;
		// Iter == end() ^ Leaf == nullptr
		virtual std::tuple<typename ListMap<K, V>::Iter, Leaf*, std::shared_ptr<Node>> find_or_new_node(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const = 0;
		// Leaf* != nullptr; std::shared_ptr<Node> != nullptr;
		virtual std::pair<Leaf*, std::shared_ptr<Node>> new_node(const HashVal hash, const std::shared_ptr<Node>& self) const = 0;
		// exists => pair(nullptr, nullptr);
		virtual std::pair<Leaf*, std::shared_ptr<Node>> new_node_if_absence(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const = 0;
		// second == true => erased;
		virtual std::pair<std::shared_ptr<Node>, bool> erased_if_exist(const HashVal hash, const K& key) const = 0;
		virtual typename ListMap<K, V>::Iter stack_right(std::vector<const Node*>& stack) const = 0;
		virtual typename ListMap<K, V>::Iter stack_left(std::vector<const Node*>& stack) const = 0;
		virtual int check_hash() const = 0;
	};
	struct Leaf final : public Node {
		ListMap<K, V> list;
		Leaf(const HashVal hash, ListMap<K, V>&& list) : Node(hash), list(std::move(list)) {};
		Leaf(const HashVal hash, const ListMap<K, V>& list) : Node(hash), list(list) {};
		Leaf(const HashVal hash) : Node(hash) {};
		Leaf(std::pair<K, V>&& pair) : Node(Hash()(pair.first)), list(std::move(pair)) {};
		const Leaf* find_or_null(const HashVal hash) const override;
		std::tuple<typename ListMap<K, V>::Iter, Leaf*, std::shared_ptr<Node>> find_or_new_node(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const override;
		std::pair<Leaf*, std::shared_ptr<Node>> new_node(const HashVal hash, const std::shared_ptr<Node>& self) const override;
		std::pair<Leaf*, std::shared_ptr<Node>> new_node_if_absence(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const override;
		std::pair<std::shared_ptr<Node>, bool> erased_if_exist(const HashVal hash, const K& key) const override;
		typename ListMap<K, V>::Iter stack_right(std::vector<const Node*>& stack) const override;
		typename ListMap<K, V>::Iter stack_left(std::vector<const Node*>& stack) const override;
		int check_hash() const override;
	};
	struct Branch final : public Node {
		std::shared_ptr<Node> left, right;
		Branch(const HashVal hash, std::shared_ptr<Node>&& left, const std::shared_ptr<Node>& right) : Node(hash), left{ std::move(left) }, right{ right }{};
		Branch(const HashVal hash, const std::shared_ptr<Node>& left, std::shared_ptr<Node>&& right) : Node(hash), left{ left }, right{ std::move(right) }{};
		Branch(std::shared_ptr<Node>&& left, const std::shared_ptr<Node>& right) : Branch(right->right_hash, std::move(left), right) {};
		Branch(const std::shared_ptr<Node>& left, std::shared_ptr<Node>&& right) : Branch(right->right_hash, left, std::move(right)) {};
		inline HashVal branch_bit() const;
		const Leaf* find_or_null(const HashVal hash) const override;
		std::tuple<typename ListMap<K, V>::Iter, Leaf*, std::shared_ptr<Node>> find_or_new_node(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const override;
		std::pair<Leaf*, std::shared_ptr<Node>> new_node(const HashVal hash, const std::shared_ptr<Node>& self) const override;
		std::pair<Leaf*, std::shared_ptr<Node>> new_node_if_absence(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const override;
		std::pair<std::shared_ptr<Node>, bool> erased_if_exist(const HashVal hash, const K& key) const override;
		typename ListMap<K, V>::Iter stack_right(std::vector<const Node*>& stack) const override;
		typename ListMap<K, V>::Iter stack_left(std::vector<const Node*>& stack) const override;
		int check_hash() const override;
	};
	size_t count;
	std::shared_ptr<Node> root;
	HashMap(const size_t count, const std::shared_ptr<Node>& root) : count{ count }, root{ root } {};
	HashMap(const size_t count, std::shared_ptr<Node>&& root) : count{ count }, root{ std::move(root) } {};
	template<typename M>
	static HashMap<K, V, Hash> make_hashmap(K&& key, M&& obj);
	template<typename M>
	static HashMap<K, V, Hash> make_hashmap(const K& key, M&& obj);

public:
	class Iter {
		std::vector<const Node*>* stack{ nullptr };
		typename ListMap<K, V>::Iter iter;
	public:
		~Iter() {
			delete stack;
		}
		Iter(const std::shared_ptr<Node>& root);
		Iter(Iter&& other) : stack{ other.stack }, iter{ other.iter } {
			other.stack = nullptr;
		}
		Iter(const Iter& other) = delete;
		Iter& operator=(Iter&& other) = default;
		Iter& operator=(const Iter& other) = delete;
		const std::pair<K, V>* operator->() const;
		const std::pair<K, V>& operator*() const;
		Iter& operator++();
		bool operator==(const Iter& other) const;
		bool operator!=(const Iter& other) const;
	};
	HashMap() : count{ 0 }, root{ nullptr }{};
	template<typename M>
	HashMap<K, V, Hash> inserted_or_updated(K&& key, M&& obj) const;
	template<typename M>
	HashMap<K, V, Hash> inserted_or_updated(const K& key, M&& obj) const;
	template<typename M>
	HashMap<K, V, Hash> inserted_if_absence(K&& key, M&& obj) const;
	template<typename M>
	HashMap<K, V, Hash> inserted_if_absence(const K& key, M&& obj) const;
	const std::pair<K, V>* get_or_null(const K& key) const;
	template<typename M>
	std::pair<const std::pair<K, V>&, HashMap<K, V, Hash>> get_or_put(const K& key, M&& obj) const;
	std::pair<const std::pair<K, V>&, HashMap<K, V, Hash>> get_or_put_default(const K& key) const;
	HashMap<K, V, Hash> erased(const K& key) const;
	size_t size() const;
	bool empty() const;
	typename Iter begin() const;
	typename Iter end() const;
	void check() const;
};
template<typename K, typename V, typename Hash>
template<typename M>
HashMap<K, V, Hash> HashMap<K, V, Hash>::make_hashmap(K&& key, M&& obj)
{
	return HashMap<K, V, Hash>(1, std::make_shared<Leaf>(std::make_pair(std::forward<K>(key), std::forward<M>(obj))));
}
template<typename K, typename V, typename Hash>
template<typename M>
HashMap<K, V, Hash> HashMap<K, V, Hash>::make_hashmap(const K& key, M&& obj)
{
	return HashMap<K, V, Hash>(1, std::make_shared<Leaf>(std::make_pair(key, std::forward<M>(obj))));
}

template<typename K, typename V, typename Hash>
template<typename M>
HashMap<K, V, Hash> HashMap<K, V, Hash>::inserted_or_updated(K&& key, M&& obj) const
{
	if (root == nullptr) return HashMap(1, std::make_shared<Leaf>(std::forward<K>(key), std::forward<M>(obj)));
	else {
		auto [leaf, new_root] = root->new_node(Hash()(key), root);
		const auto [iter, rest] = leaf->list.erased_first(key);
		leaf->list = rest.appended(std::forward<K>(key), std::forward<M>(obj));
		return HashMap(count + (iter.is_end() ? 1 : 0), std::move(new_root));
	}
}

template<typename K, typename V, typename Hash>
template<typename M>
HashMap<K, V, Hash> HashMap<K, V, Hash>::inserted_or_updated(const K& key, M&& obj) const
{
	if (root == nullptr) return HashMap::make_hashmap(key, std::forward<M>(obj));
	else {
		auto [leaf, new_root] = root->new_node(Hash()(key), root);
		const auto [iter, rest] = leaf->list.erased_first(key);
		leaf->list = rest.appended(key, std::forward<M>(obj));
		return HashMap(count + (iter.is_end() ? 1 : 0), std::move(new_root));
	}
}

template<typename K, typename V, typename Hash>
template<typename M>
HashMap<K, V, Hash> HashMap<K, V, Hash>::inserted_if_absence(K&& key, M&& obj) const
{
	if (root == nullptr) return HashMap::make_hashmap(std::forward<K>(key), std::forward<M>(obj));
	else {
		auto [leaf, new_root] = root->new_node_if_absence(Hash()(key), key, root);
		if (leaf == nullptr) return *this;
		else {
			leaf->list = leaf->list.appended(std::forward<K>(key), std::forward<M>(obj));
			return HashMap(count + 1, std::move(new_root));
		}
	}
}

template<typename K, typename V, typename Hash>
template<typename M>
HashMap<K, V, Hash> HashMap<K, V, Hash>::inserted_if_absence(const K& key, M&& obj) const
{
	if (root == nullptr) return HashMap::make_hashmap(key, std::forward<M>(obj));
	else {
		auto [leaf, new_root] = root->new_node_if_absence(Hash()(key), key, root);
		if (leaf == nullptr) return *this;
		else {
			leaf->list = leaf->list.appended(key, std::forward<M>(obj));
			return HashMap(count + 1, std::move(new_root));
		}
	}
}

template<typename K, typename V, typename Hash>
template<typename M>
std::pair<const std::pair<K, V>&, HashMap<K, V, Hash>> HashMap<K, V, Hash>::get_or_put(const K& key, M&& obj) const
{
	if (root == nullptr) {
		auto map = HashMap::make_hashmap(key, std::forward<M>(obj));
		auto kv = map.get_or_null(key);
		return std::make_pair(*kv, std::move(map));
	}
	else {
		auto [iter, leaf, new_root] = root->find_or_new_node(Hash()(key), key, root);
		if (leaf == nullptr) {
			return std::make_pair(*iter, *this);
		}
		else {
			leaf->list = leaf->list.appended(key, std::forward<M>(obj));
			return std::make_pair(leaf->list.head(), HashMap{ count + 1, std::move(new_root) });
		}
	}
}

template<typename K, typename V, typename Hash>
std::pair<const std::pair<K, V>&, HashMap<K, V, Hash>> HashMap<K, V, Hash>::get_or_put_default(const K& key) const
{
	if (root == nullptr) {
		auto map = HashMap::make_hashmap(key, V());
		auto kv = map.get_or_null(key);
		return std::make_pair(*kv, std::move(map));
	}
	else {
		auto [iter, leaf, new_root] = root->find_or_new_node(Hash()(key), key, root);
		if (leaf == nullptr) {
			return std::make_pair(*iter, *this);
		}
		else {
			leaf->list = leaf->list.appended(key, V());
			return std::make_pair(leaf->list.head(), HashMap{ count + 1, std::move(new_root) });
		}
	}
}

template<typename K, typename V, typename Hash>
const std::pair<K, V>* HashMap<K, V, Hash>::get_or_null(const K& key) const
{
	if (root == nullptr) return nullptr;
	else {
		auto leaf = root->find_or_null(Hash()(key));
		if (leaf == nullptr) return nullptr;
		else  return leaf->list.find_first(key).get();
	}
}

template<typename K, typename V, typename Hash>
HashMap<K, V, Hash> HashMap<K, V, Hash>::erased(const K& key) const
{
	if (root == nullptr) return *this;
	else {
		auto [new_root, b] = root->erased_if_exist(Hash()(key), key);
		if (!b) return *this;
		else return HashMap(count - 1, std::move(new_root));
	}
}

template<typename K, typename V, typename Hash>
size_t HashMap<K, V, Hash>::size() const
{
	return count;
}

template<typename K, typename V, typename Hash>
bool HashMap<K, V, Hash>::empty() const
{
	return count == 0;
}

template<typename K, typename V, typename Hash>
typename HashMap<K, V, Hash>::Iter HashMap<K, V, Hash>::begin() const
{
	return Iter(root);
}

template<typename K, typename V, typename Hash>
typename HashMap<K, V, Hash>::Iter HashMap<K, V, Hash>::end() const
{
	return Iter(nullptr);
}

template<typename K, typename V, typename Hash>
void HashMap<K, V, Hash>::check() const
{
	if (root == nullptr) assert(count == 0);
	else assert(count == root->check_hash());
}

template<typename K, typename V, typename Hash>
inline typename HashMap<K, V, Hash>::HashVal HashMap<K, V, Hash>::Branch::branch_bit() const
{
	return Node::right_bit(this->right_hash ^ left->right_hash);
}

template<typename K, typename V, typename Hash>
const typename HashMap<K, V, Hash>::Leaf* HashMap<K, V, Hash>::Branch::find_or_null(const HashVal hash) const
{
	const auto branch = branch_bit();
	if ((branch - 1) & (this->right_hash ^ hash)) {
		return nullptr;
	}
	else {
		if (branch & hash) {
			return right->find_or_null(hash);
		}
		else {
			return left->find_or_null(hash);
		}
	}
}

template<typename K, typename V, typename Hash>
std::tuple<typename ListMap<K, V>::Iter, typename HashMap<K, V, Hash>::Leaf*, std::shared_ptr<typename HashMap<K, V, Hash>::Node>> HashMap<K, V, Hash>::Branch::find_or_new_node(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const
{
	const auto branch = branch_bit();
	if ((branch - 1) & (this->right_hash ^ hash)) {
		if (Node::right_bit(this->right_hash ^ hash) & hash) {
			auto new_right = std::make_shared<Leaf>(hash);
			auto leaf = new_right.get();
			return std::make_tuple(new_right->list.begin(), leaf, std::make_shared<Branch>(hash, self, std::move(new_right)));
		}
		else {
			auto new_left = std::make_shared<Leaf>(hash);
			auto leaf = new_left.get();
			return std::make_tuple(new_left->list.begin(), leaf, std::make_shared<Branch>(this->right_hash, std::move(new_left), self));
		}
	}
	else {
		if (branch & hash) {
			auto [iter, leaf, r] = right->find_or_new_node(hash, key, right);
			if (leaf == nullptr) return std::make_tuple(std::move(iter), nullptr, nullptr);
			else return std::make_tuple(std::move(iter), leaf, std::make_shared<Branch>(left, std::move(r)));
		}
		else {
			auto [iter, leaf, l] = left->find_or_new_node(hash, key, left);
			if (leaf == nullptr) return std::make_tuple(std::move(iter), nullptr, nullptr);
			else return std::make_tuple(std::move(iter), leaf, std::make_shared<Branch>(this->right_hash, std::move(l), right));
		}
	}
}

template<typename K, typename V, typename Hash>
std::pair<typename HashMap<K, V, Hash>::Leaf*, std::shared_ptr<typename HashMap<K, V, Hash>::Node>> HashMap<K, V, Hash>::Branch::new_node(const HashVal hash, const std::shared_ptr<Node>& self) const
{
	const auto branch = branch_bit();
	if ((branch - 1) & (this->right_hash ^ hash)) {
		if (Node::right_bit(this->right_hash ^ hash) & hash) {
			auto new_right = std::make_shared<Leaf>(hash);
			auto leaf = new_right.get();
			return std::make_pair(leaf, std::make_shared<Branch>(hash, self, std::move(new_right)));
		}
		else {
			auto new_left = std::make_shared<Leaf>(hash);
			auto leaf = new_left.get();
			return std::make_pair(leaf, std::make_shared<Branch>(this->right_hash, std::move(new_left), self));
		}
	}
	else {
		if (branch & hash) {
			auto [leaf, new_right] = right->new_node(hash, right);
			return std::make_pair(leaf, std::make_shared<Branch>(left, std::move(new_right)));
		}
		else {
			auto [leaf, new_left] = left->new_node(hash, left);
			return std::make_pair(leaf, std::make_shared<Branch>(this->right_hash, std::move(new_left), right));
		}
	}
}

template<typename K, typename V, typename Hash>
std::pair<typename HashMap<K, V, Hash>::Leaf*, std::shared_ptr<typename HashMap<K, V, Hash>::Node>> HashMap<K, V, Hash>::Branch::new_node_if_absence(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const
{
	const auto branch = branch_bit();
	if ((branch - 1) & (this->right_hash ^ hash)) {
		if (Node::right_bit(this->right_hash ^ hash) & hash) {
			auto new_right = std::make_shared<Leaf>(hash);
			auto leaf = new_right.get();
			return std::make_pair(leaf, std::make_shared<Branch>(hash, self, std::move(new_right)));
		}
		else {
			auto new_left = std::make_shared<Leaf>(hash);
			auto leaf = new_left.get();
			return std::make_pair(leaf, std::make_shared<Branch>(this->right_hash, std::move(new_left), self));
		}
	}
	else {
		if (branch & hash) {
			auto [leaf, r] = right->new_node_if_absence(hash, key, right);
			if (leaf == nullptr) return std::make_pair(nullptr, nullptr);
			else return std::make_pair(leaf, std::make_shared<Branch>(left, std::move(r)));
		}
		else {
			auto [leaf, l] = left->new_node_if_absence(hash, key, left);
			if (leaf == nullptr) return std::make_pair(nullptr, nullptr);
			else return std::make_pair(leaf, std::make_shared<Branch>(this->right_hash, std::move(l), right));
		}
	}
}

template<typename K, typename V, typename Hash>
std::pair<std::shared_ptr<typename HashMap<K, V, Hash>::Node>, bool> HashMap<K, V, Hash>::Branch::erased_if_exist(const HashVal hash, const K& key) const
{
	const auto branch = branch_bit();
	if ((branch - 1) & (this->right_hash ^ hash)) return std::make_pair(nullptr, false);
	else {
		if (branch & hash) {
			auto [r, b] = right->erased_if_exist(hash, key);
			if (!b) return std::make_pair(nullptr, false);
			else if (r == nullptr) return std::make_pair(left, true);
			else return std::make_pair(std::make_shared<Branch>(left, std::move(r)), true);
		}
		else {
			auto [l, b] = left->erased_if_exist(hash, key);
			if (!b) return std::make_pair(nullptr, false);
			else if (l == nullptr) return std::make_pair(right, true);
			else return std::make_pair(std::make_shared<Branch>(this->right_hash, std::move(l), right), true);
		}
	}
}

template<typename K, typename V, typename Hash>
typename ListMap<K, V>::Iter HashMap<K, V, Hash>::Branch::stack_right(std::vector<const Node*>& stack) const
{
	return right->stack_left(stack);
}

template<typename K, typename V, typename Hash>
typename ListMap<K, V>::Iter HashMap<K, V, Hash>::Branch::stack_left(std::vector<const Node*>& stack) const
{
	stack.push_back(this);
	return left->stack_left(stack);
}

template<typename K, typename V, typename Hash>
int HashMap<K, V, Hash>::Branch::check_hash() const
{
	assert(this->right_hash == right->right_hash);
	assert((Node::right_bit(this->right_hash ^ left->right_hash) & this->right_hash) > 0);
	assert((Node::right_bit(this->right_hash ^ left->right_hash) & left->right_hash) == 0);
	return right->check_hash() + left->check_hash();
}

template<typename K, typename V, typename Hash>
inline typename HashMap<K, V, Hash>::HashVal HashMap<K, V, Hash>::Node::right_bit(const HashVal hash)
{
	return hash & (~hash + 1);
}

template<typename K, typename V, typename Hash>
const typename HashMap<K, V, Hash>::Leaf* HashMap<K, V, Hash>::Leaf::find_or_null(const HashVal hash) const
{
	if (this->right_hash == hash) {
		return this;
	}
	else {
		return nullptr;
	}
}

template<typename K, typename V, typename Hash>
std::tuple<typename ListMap<K, V>::Iter, typename HashMap<K, V, Hash>::Leaf*, std::shared_ptr<typename HashMap<K, V, Hash>::Node>> HashMap<K, V, Hash>::Leaf::find_or_new_node(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const
{
	if (this->right_hash == hash) {
		auto find = list.find_first(key);
		if (find != list.end()) return std::make_tuple(std::move(find), nullptr, nullptr);
		else {
			auto new_leaf = std::make_shared<Leaf>(hash);
			new_leaf->list = list;
			auto leaf = new_leaf.get();
			return std::make_tuple(std::move(find), leaf, std::move(new_leaf));
		}
	}
	else {
		auto new_leaf = std::make_shared<Leaf>(hash);
		auto iter = new_leaf->list.end();
		auto leaf = new_leaf.get();
		if (Node::right_bit(this->right_hash ^ hash) & hash) {
			return std::make_tuple(std::move(iter), leaf, std::make_shared<Branch>(hash, self, std::move(new_leaf)));
		}
		else {
			return std::make_tuple(std::move(iter), leaf, std::make_shared<Branch>(this->right_hash, std::move(new_leaf), self));
		}
	}
}

template<typename K, typename V, typename Hash>
std::pair<typename HashMap<K, V, Hash>::Leaf*, std::shared_ptr<typename HashMap<K, V, Hash>::Node>> HashMap<K, V, Hash>::Leaf::new_node(const HashVal hash, const std::shared_ptr<Node>& self) const
{
	if (this->right_hash == hash) {
		auto new_leaf = std::make_shared<Leaf>(hash, list);
		auto leaf = new_leaf.get();
		return std::make_pair(leaf, std::move(new_leaf));
	}
	else {
		auto new_leaf = std::make_shared<Leaf>(hash);
		auto leaf = new_leaf.get();
		if (Node::right_bit(this->right_hash ^ hash) & hash) {
			return std::make_pair(leaf, std::make_shared<Branch>(hash, self, std::move(new_leaf)));
		}
		else {
			return std::make_pair(leaf, std::make_shared<Branch>(this->right_hash, std::move(new_leaf), self));
		}
	}
}

template<typename K, typename V, typename Hash>
std::pair<typename HashMap<K, V, Hash>::Leaf*, std::shared_ptr<typename HashMap<K, V, Hash>::Node>> HashMap<K, V, Hash>::Leaf::new_node_if_absence(const HashVal hash, const K& key, const std::shared_ptr<Node>& self) const
{
	if (this->right_hash == hash) {
		auto find = list.find_first(key);
		if (find == list.end()) {
			auto new_leaf = std::make_shared<Leaf>(hash, list);
			auto leaf = new_leaf.get();
			return std::make_pair(leaf, std::move(new_leaf));
		}
		else {
			return std::make_pair(nullptr, nullptr);
		}
	}
	else {
		auto new_leaf = std::make_shared<Leaf>(hash);
		auto leaf = new_leaf.get();
		if (Node::right_bit(this->right_hash ^ hash) & hash) {
			return std::make_pair(leaf, std::make_shared<Branch>(hash, self, std::move(new_leaf)));
		}
		else {
			return std::make_pair(leaf, std::make_shared<Branch>(this->right_hash, std::move(new_leaf), self));
		}
	}
}

template<typename K, typename V, typename Hash>
std::pair<std::shared_ptr<typename HashMap<K, V, Hash>::Node>, bool> HashMap<K, V, Hash>::Leaf::erased_if_exist(const HashVal hash, const K& key) const
{
	if (this->right_hash == hash) {
		auto [iter, rest] = list.erased_first(key);
		if (iter == list.end()) return std::make_pair(nullptr, false);
		else if (rest.empty()) return std::make_pair(nullptr, true);
		else return std::make_pair(std::make_shared<Leaf>(hash, rest), true);
	}
	else return std::make_pair(nullptr, false);
}

template<typename K, typename V, typename Hash>
typename ListMap<K, V>::Iter HashMap<K, V, Hash>::Leaf::stack_right(std::vector<const Node*>& stack) const
{
	return list.end();
}

template<typename K, typename V, typename Hash>
typename ListMap<K, V>::Iter HashMap<K, V, Hash>::Leaf::stack_left(std::vector<const Node*>& stack) const
{
	return list.begin();
}

template<typename K, typename V, typename Hash>
int HashMap<K, V, Hash>::Leaf::check_hash() const
{
	int count{ 0 };
	for (const auto [key, _] : list) {
		assert(Hash()(key) == this->right_hash);
		++count;
	}
	return count;
}

template<typename K, typename V, typename Hash>
HashMap<K, V, Hash>::Iter::Iter(const std::shared_ptr<Node>& root)
{
	if (root != nullptr) {
		stack = new std::vector<const Node*>();
		iter = root->stack_left(*stack);
		if (stack->empty()) {
			delete stack;
			stack = nullptr;
		}
	}
	else {
		iter.skip_to_end();
	}
}

template<typename K, typename V, typename Hash>
const std::pair<K, V>* HashMap<K, V, Hash>::Iter::operator->() const
{
	return iter.get();
}

template<typename K, typename V, typename Hash>
const std::pair<K, V>& HashMap<K, V, Hash>::Iter::operator*() const
{
	return *iter;
}

template<typename K, typename V>
bool ListMap<K, V>::Iter::operator==(const Iter& other) const {
	return current == other.current;
}
template<typename K, typename V>
bool ListMap<K, V>::Iter::operator!=(const Iter& other) const {
	return current != other.current;
}
template<typename K, typename V>
typename ListMap<K, V>::Iter& ListMap<K, V>::Iter::operator++() {
	current = current->tail.get();
	return *this;
}

template<typename K, typename V>
const std::pair<K, V>* ListMap<K, V>::Iter::operator->() const {

}
template<typename K, typename V>
const std::pair<K, V>& ListMap<K, V>::Iter::operator*() const {
	return *current->head;
}

template<typename K, typename V>
const std::pair<K, V>* ListMap<K, V>::Iter::get() const
{
	return current->head.get();
}

template<typename K, typename V>
bool ListMap<K, V>::Iter::is_end() const
{
	return current == nullptr;
}

template<typename K, typename V>
void ListMap<K, V>::Iter::skip_to_end()
{
	current = nullptr;
}

template<typename K, typename V, typename Hash>
typename HashMap<K, V, Hash>::Iter& HashMap<K, V, Hash>::Iter::operator++()
{
	++iter;
	if (iter.is_end()) {
		if (stack != nullptr && !stack->empty()) {
			auto last = stack->back();
			stack->pop_back();
			iter = last->stack_right(*stack);
			if (stack->empty()) {
				delete stack;
				stack = nullptr;
			}
		}
	}
	return *this;
}

template<typename K, typename V, typename Hash>
bool HashMap<K, V, Hash>::Iter::operator==(const Iter& other) const
{
	return stack == other.stack && iter == other.iter;
}

template<typename K, typename V, typename Hash>
bool HashMap<K, V, Hash>::Iter::operator!=(const Iter& other) const
{
	return !(*this == other);
}

template<typename K, typename V>
template<typename M>
ListMap<K, V> ListMap<K, V>::appended(K&& key, M&& obj) const
{
	return ListMap<K, V>(std::make_shared<Node>(std::make_pair(std::forward<K>(key), std::forward<M>(obj)), list));
}

template<typename K, typename V>
template<typename M>
ListMap<K, V> ListMap<K, V>::appended(const K& key, M&& obj) const
{
	return ListMap<K, V>(std::make_shared<Node>(std::make_shared<std::pair<K, V>>(key, std::forward<M>(obj)), list));
}

template<typename K, typename V>
typename ListMap<K, V>::Iter ListMap<K, V>::begin() const
{
	return Iter{ list.get() };
}

template<typename K, typename V>
typename ListMap<K, V>::Iter ListMap<K, V>::end() const
{
	return Iter{ nullptr };
}

template<typename K, typename V>
const std::pair<K, V>& ListMap<K, V>::head() const
{
	return *list->head;
}

template<typename K, typename V>
ListMap<K, V> ListMap<K, V>::appended(std::pair<K, V>&& pair) const
{
	return ListMap<K, V>(std::make_shared<Node>(std::move(pair), list));
}

template<typename K, typename V>
ListMap<K, V> ListMap<K, V>::appended(const std::pair<K, V>& pair) const
{
	return ListMap<K, V>(std::make_shared<Node>(pair, list));
}

template<typename K, typename V>
typename ListMap<K, V>::Iter ListMap<K, V>::find_first(const K& key) const
{
	auto ptr = list.get();
	while (ptr != nullptr && ptr->head->first != key) {
		ptr = ptr->tail.get();
	}
	return Iter{ ptr };
}

template<typename K, typename V>
std::pair<typename ListMap<K, V>::Iter, ListMap<K, V>> ListMap<K, V>::erased_first(const K& key) const
{
	std::stack<Node*> stack;
	auto ptr = list.get();
	while (ptr != nullptr && ptr->head->first != key) {
		stack.push(ptr);
		ptr = ptr->tail.get();
	}
	if (ptr == nullptr) return std::make_pair(end(), *this);
	else {
		auto new_list = ptr->tail;
		while (!stack.empty()) {
			new_list = std::make_shared<Node>(stack.top()->head, new_list);
			stack.pop();
		}
		return std::make_pair(Iter{ ptr }, ListMap{ std::move(new_list) });
	}
}

template<typename K, typename V>
bool ListMap<K, V>::empty() const
{
	return list == nullptr;
}



template<typename K, typename V>
class Tester {
	std::unordered_map<K, V> map;
	Tester(std::unordered_map<K, V>&& map) : map{ std::move(map) } {};
public:
	Tester() {};
	template<typename M>
	Tester<K, V> inserted_or_updated(K&& key, M&& obj) const;
	template<typename M>
	Tester<K, V> inserted_or_updated(const K& key, M&& obj) const;
	template<typename M>
	Tester<K, V> inserted_if_absence(K&& key, M&& obj) const;
	template<typename M>
	Tester<K, V> inserted_if_absence(const K& key, M&& obj) const;
	const std::pair<const K, V>* get_or_null(const K& key) const;
	template<typename M>
	std::pair<std::pair<K, V>, Tester<K, V>> get_or_put(const K& key, M&& obj) const;
	std::pair<std::pair<K, V>, Tester<K, V>> get_or_put_default(const K& key) const;
	Tester<K, V> erased(const K& key) const;
	size_t size() const;
	bool empty() const;
};

template<typename K, typename V>
template<typename M>
Tester<K, V> Tester<K, V>::inserted_or_updated(K&& key, M&& obj) const
{
	auto copy = map;
	copy.insert(std::make_pair(std::forward<K>(key), std::forward<M>(obj)));
	return Tester{ std::move(copy) };
}

template<typename K, typename V>
template<typename M>
Tester<K, V> Tester<K, V>::inserted_or_updated(const K& key, M&& obj) const
{
	auto copy = map;
	copy.insert(std::make_pair(key, std::forward<M>(obj)));
	return Tester{ std::move(copy) };
}

template<typename K, typename V>
template<typename M>
Tester<K, V> Tester<K, V>::inserted_if_absence(K&& key, M&& obj) const
{
	auto find = map.find(key);
	if (find == map.end()) {
		auto copy = map;
		copy.insert(std::make_pair(std::forward<K>(key), std::forward<M>(obj)));
		return Tester{ std::move(copy) };
	}
	else {
		return *this;
	}
}
template<typename K, typename V>
template<typename M>
Tester<K, V> Tester<K, V>::inserted_if_absence(const K& key, M&& obj) const
{
	auto find = map.find(key);
	if (find == map.end()) {
		auto copy = map;
		copy.insert(std::make_pair(key, std::forward<M>(obj)));
		return Tester{ std::move(copy) };
	}
	else {
		return *this;
	}
}

template<typename K, typename V>
template<typename M>
std::pair<std::pair<K, V>, Tester<K, V>> Tester<K, V>::get_or_put(const K& key, M&& obj) const
{
	auto find = map.find(key);
	if (find == map.end()) {
		auto copy = map;
		auto p = std::make_pair(key, std::forward<M>(obj));
		copy.insert(p);
		return std::make_pair(std::move(p), Tester{ std::move(copy) });
	}
	else {
		return std::make_pair(*find, *this);
	}
}

template<typename K, typename V>
const std::pair<const K, V>* Tester<K, V>::get_or_null(const K& key) const
{
	auto find = map.find(key);
	if (find == map.end()) return nullptr;
	else return find.operator->();
}

template<typename K, typename V>
std::pair<std::pair<K, V>, Tester<K, V>> Tester<K, V>::get_or_put_default(const K& key) const
{
	auto copy = map;
	auto p = copy.find(key);
	return std::make_pair(*p, Tester{ std::move(copy) });
}

template<typename K, typename V>
Tester<K, V> Tester<K, V>::erased(const K& key) const
{
	auto copy = map;
	copy.erase(key);
	return Tester<K, V>(std::move(copy));
}

template<typename K, typename V>
size_t Tester<K, V>::size() const
{
	return map.size();
}

template<typename K, typename V>
bool Tester<K, V>::empty() const
{
	return map.empty();
}

struct H {
	int operator()(const int i) const {
		return i / 10;
	}
};

void self_check(HashMap<int, int, H> hash_map) {
	std::cout << "self check, size: " << hash_map.size() << '\n';
	hash_map.check();
	std::vector<std::pair<int, int>> vec;
	for (const auto p : hash_map) {
		vec.push_back(p);
	}
	std::shuffle(vec.begin(), vec.end(), std::mt19937(0));
	int last_size = hash_map.size();
	for (const auto p : vec) {
		assert(last_size == hash_map.size());
		auto erased = hash_map.erased(p.first);
		assert(last_size == erased.size() + 1);
		assert(hash_map.get_or_null(p.first)->second == p.second);
		assert(erased.get_or_null(p.first) == nullptr);
		hash_map = erased;
		last_size = erased.size();
	}
	assert(hash_map.empty());
}
void check_get(HashMap<int, int, H>& map, Tester<int, int>& test, int key) {
	auto p = map.get_or_null(key);
	auto p2 = test.get_or_null(key);
	assert(((p == nullptr) && (p2 == nullptr)) || ((p->second == p2->second) && (p->first == p2->first)));
	map.check();
}
void insert_or_put_check(HashMap<int, int, H>& map, Tester<int, int>& test, int key, int value) {
	check_get(map, test, key);
	auto inserted = map.inserted_or_updated(key, value);
	auto inserted2 = test.inserted_or_updated(key, value);
	assert(inserted.get_or_null(key)->second == inserted2.get_or_null(key)->second);
	assert(inserted.get_or_null(key)->first == inserted2.get_or_null(key)->first);
	assert(map.size() == test.size() && inserted.size() == inserted2.size());
	map = std::move(inserted);
	test = std::move(inserted2);
}
void insert_if_absence_check(HashMap<int, int, H>& map, Tester<int, int>& test, int key, int value) {
	check_get(map, test, key);
	auto inserted = map.inserted_if_absence(key, value);
	auto inserted2 = test.inserted_if_absence(key, value);
	assert(inserted.get_or_null(key)->second == inserted2.get_or_null(key)->second);
	assert(inserted.get_or_null(key)->first == inserted2.get_or_null(key)->first);
	assert(map.size() == test.size() && inserted.size() == inserted2.size());
	map = std::move(inserted);
	test = std::move(inserted2);
}
void erase_check(HashMap<int, int, H>& map, Tester<int, int>& test, int key) {
	check_get(map, test, key);
	auto erased = map.erased(key);
	auto erased2 = test.erased(key);
	assert(erased.get_or_null(key) == nullptr && erased2.get_or_null(key) == nullptr);
	assert(map.size() == test.size() && erased.size() == erased2.size());
	map = std::move(erased);
	test = std::move(erased2);
}

void check(HashMap<int, int, H> map, Tester<int, int> tester, std::mt19937& rand, int depth = 0) {
	constexpr int max_value = 30;
	constexpr int max_depth = 20;
	constexpr int max_loop = 50;
	if (depth == max_depth) return;
	self_check(map);
	for (auto i = 0; i < max_loop; ++i) {
		int op = rand() % 4;
		int value = rand() % max_value;
		switch (op) {
		case 0: insert_or_put_check(map, tester, value, -value); break;
		case 1: erase_check(map, tester, value); break;
		case 2: check_get(map, tester, value); break;
		case 3: insert_if_absence_check(map, tester, value, -value); break;
		default: throw 0;
		}
		map.check();
	}
	for (auto op = 0; op < 4; ++op) {
		auto temp = map;
		auto temp2 = tester;
		int value = rand() % max_value;
		switch (op) {
		case 0: insert_or_put_check(map, tester, value, -value); break;
		case 1: erase_check(map, tester, value); break;
		case 2: check_get(map, tester, value); break;
		case 3: insert_if_absence_check(map, tester, value, -value); break;
		default: throw 0;
		}
		check(map, tester, rand, depth + 1);
		map = temp;
		tester = temp2;
	}
}


template<typename F>
long long int calc_time(const F& func, const std::string message, const int repeat = 3) {
	auto start = std::chrono::system_clock::now();
	for (auto i = 0; i < repeat; ++i) {
		func();
	}
	auto end = std::chrono::system_clock::now();
	auto diff = end - start;
	std::cout << message << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << '\n';
	return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
}
template<typename K, typename V>
void check_hashmap(const std::vector<std::pair<K, V>>& vec) {
	HashMap<K, V> map;
	for (const auto [key, value] : vec) {
		map = map.inserted_or_updated(key, value);
	}
	if (vec.size() != map.size()) {
		throw 0;
	}
}
template<typename K, typename V>
void check_map(const std::vector<std::pair<K, V>>& vec) {
	std::unordered_map<K, V> map;
	for (const auto [key, value] : vec) {
		map.insert_or_assign(key, value);
	}
	if (vec.size() != map.size()) {
		throw 0;
	}
}

void property_check() {
	HashMap<int, int, H> map;
	Tester<int, int> tester;
	std::mt19937 rand(1);
	check(map, tester, rand);
}
}