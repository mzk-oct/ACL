






#include <utility>
namespace {
	template<typename T>
	T off_set();
	template<typename T>
	T prime();
	template<typename T>
	void combine_hash(T& Hash, const T& add) {
		Hash ^= add;
		Hash *= prime<T>();
	}
	template<typename V, typename T>
	T Hash(const V& value);


	template<typename F, typename S, typename T>
	T Hash(const std::pair<F, S>& pair) {
		T result{ off_set<T>() };
		combine_hash(result, Hash<F, T>(pair.first));
		combine_hash(result, Hash<S, T>(pair.second));
	}
	template<>
	unsigned long long int off_set<unsigned long long int>() {
		return 14695981039346656037ULL;
	}
	template<>
	void combine_hash<unsigned long long int>(unsigned long long int& Hash, const unsigned long long int &add) {
		Hash ^= add;
		Hash *= 1099511628211LL;
	}
}