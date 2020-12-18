#include <vector>
#include <array>;
namespace {
	constexpr int MOD = 998244353;

	static std::array<int, 20> primitive_root{ 0, 998244352, 911660635, 372528824, 69212480, 617152567, 926351381, 553154607, 835711138, 476870429, 578036328, 579795361, 533803820, 843506711, 57929585, 670729537, 341689982, 178698007, 3411409, 1847 };
	static std::array<int, 20> reverse_root{ 0, 998244352, 86583718, 509520358, 661054123, 134322755, 77113778, 871797313, 179557483, 757331369, 195869564, 981311075, 229697978, 936970679, 760640816, 113194219, 815372449, 798661279, 53782855, 510742238 };

	std::vector<int> fold_loop(const std::vector<int>& a, const std::vector<int>& b) {
		if (a.size() >= b.size()) {
			std::vector<int> result(a.size() + b.size() - 1, 0);
			for (auto i = 0; i < b.size(); ++i) {
				for (auto j = 0; j < a.size(); ++j) {
					result[i + j] = (result[i + j] + (long long int)b[i] * a[j]) % MOD;
				}
			}
			return result;
		}
		else return fold_loop(b, a);
	}
	int log2ceil(int size) {
		int log{ 0 };
		while (size > 1) {
			size = (size + 1) >> 1;
			++log;
		}
		return log;
	}
	long long int power(long long int base, int exp) {
		long long int result{ 1 };
		while (exp > 0) {
			if (exp & 1) {
				result = result * base % MOD;
			}
			base = (base * base) % MOD;
			exp >>= 1;
		}
		return result;
	}
	std::vector<int> ft(std::vector<int> a, const bool inverse = false) {
		const auto log = log2ceil(a.size());
		for (auto i = a.size(); i < (1 << log); ++i) a.push_back(0);
		std::vector<int> result(a.size(), 0);
		for (auto i = 0; i < result.size(); ++i) {
			const auto root = inverse ? power(reverse_root[log], i) : power(primitive_root[log], i);
			auto r = 1LL;
			for (auto j = 0; j < a.size(); ++j) {
				result[i] = (result[i] + a[j] * r) % MOD;
				r = (r * root) % MOD;
			}
			result[i] %= MOD;
		}
		if (inverse) {
			const auto rev = power(a.size(), MOD - 2);
			for (auto& n : result) n = (n * rev) % MOD;
		}
		return result;
	}
	std::vector<int> fft(std::vector<int> a, const bool inverse = false) {
		const auto log = log2ceil(a.size());
		a.reserve(1 << log);
		for (auto i = a.size(); i < (1 << log); ++i) {
			a.push_back(0);
		}
		for (auto i = 0; i < a.size(); ++i) {
			int rev{ 0 };
			for (auto j = 0; j < log; ++j) {
				rev <<= 1;
				rev += (i >> j) & 1;
			}
			if (i < rev) std::swap(a[i], a[rev]);
		}
		for (auto d = 0; (1 << d) < a.size(); ++d) {
			const auto width = 1 << d;
			auto root = 1LL;
			const auto half = inverse ? reverse_root[1] : primitive_root[1];
			const auto w = inverse ? reverse_root[d + 1] : primitive_root[d + 1];
			for (auto i = 0; i < width; ++i) {
				for (auto j = 0; j < a.size(); j += width << 1) {
					const auto pos = a[i + j + width] * root % MOD;
					a[i + j + width] = (a[i + j] + pos * half % MOD) % MOD;
					a[i + j] = (a[i + j] + pos) % MOD;
				}
				root = root * w % MOD;
			}
		}
		if (inverse) {
			const auto rev = power(a.size(), MOD - 2);
			for (auto& x : a) x = (x * rev) % MOD;
		}
		return a;
	}
	std::vector<int> fold_fft(std::vector<int> a, std::vector<int>& b) {
		const auto log = log2ceil(a.size() + b.size() - 1);
		for (auto i = a.size(); i < (1 << log); ++i) {
			a.push_back(0);
		}
		const auto a_fft = fft(a);
		for (auto i = b.size(); i < (1 << log); ++i) {
			b.push_back(0);
		}
		const auto b_fft = fft(b);
		std::vector<int> c(1 << log);
		for (auto i = 0; i < c.size(); ++i) {
			c[i] = (long long int)a_fft[i] * b_fft[i] % MOD;
		}
		const auto result = fft(c, true);
		return result;
	}
	std::vector<int> fold(const std::vector<int>& a, const std::vector<int>& b) {
		if (a.size() <= 10 || b.size() <= 10) return fold_loop(a, b);
		return {};
	}
}