#include <vector>
#include <complex>

namespace {
	int log2ceil(int n) {
		int result{ 0 };
		while (n > 1) {
			++result;
			n = (n + 1) >> 1;
		}
		return result;
	}

	using Vec = std::vector<std::complex<double>>;

	Vec operator*(const Vec& vec, const std::complex<double> value) {
		auto result = vec;
		for (auto& v : result) v *= value;
		return result;
	}
	Vec operator/(const Vec& vec, const std::complex<double> value) {
		auto result = vec;
		for (auto& v : result) v /= value;
		return result;
	}
	Vec& operator/=(Vec& vec, const std::complex<double> other) {
		for (auto i = 0; i < vec.size(); ++i) {
			vec[i] /= other;
		}
		return vec;
	}
	Vec operator+(const Vec& vec, const Vec& right) {
		auto result = vec;
		for (auto i = 0; i < result.size(); ++i) {
			result[i] += right[i];
		}
		return result;
	}
	Vec operator-(const Vec& vec, const Vec& right) {
		auto result = vec;
		for (auto i = 0; i < result.size(); ++i) {
			result[i] -= right[i];
		}
		return result;
	}
	Vec& operator+=(Vec& vec, const Vec& other) {
		for (auto i = 0; i < vec.size(); ++i) {
			vec[i] += other[i];
		}
		return vec;
	}

	template<typename F>
	std::vector<F> fft(const std::vector<F>& vec, const bool inverse = false) {
		const auto PI = std::acos(-1);
		const auto log2 = log2ceil(vec.size());
		auto result = vec;
		result.resize(1 << log2);
		for (auto i = 0; i < result.size(); ++i) {
			int j = 0;
			for (auto d = 0; d < log2; ++d) {
				j <<= 1;
				j += (i >> d) & 1;
			}
			if (i < j) std::swap(result[i], result[j]);
		}
		for (auto b = 0; b < log2; ++b) {
			const auto width = 1 << b;
			for (auto r = 0; r < width; ++r) {
				const auto w = std::complex<double>(std::cos(r * PI / width), std::sin(r * PI * (inverse ? 1 : -1) / width));
				for (auto i = r; i < result.size(); i += width << 1) {
					const auto t = result[i + width] * w;
					result[i + width] = result[i] - t;
					result[i] += t;
				}
			}
		}
		if (inverse) {
			for (auto& v : result) v /= result.size();
		}
		return result;
	}

	Vec to_vec(const std::vector<int>& a) {
		Vec result(a.size());
		for (auto i = 0; i < result.size(); ++i) {
			result[i] = std::complex<double>(a[i], 0);
		}
		return result;
	}
	std::vector<Vec> to_vec(const std::vector<std::vector<int>>& a) {
		std::vector<Vec> result(a.size());
		for (auto i = 0; i < a.size(); ++i) {
			result[i] = to_vec(a[i]);
		}
		return result;
	}
	void resize(std::vector<Vec>& a, const int height, const int width) {
		a.resize(height);
		for (auto& v : a) v.resize(width);
	}

	std::vector<std::vector<double>> fold_fft(const std::vector<std::vector<int>> a, const std::vector<std::vector<int>> b) {
		const auto height = a.size() + b.size() - 1;
		const auto width = a[0].size() + b[0].size() - 1;
		auto vec_a = to_vec(a); resize(vec_a, 1 << log2ceil(height), 1 << log2ceil(width));
		auto vec_b = to_vec(b); resize(vec_b, 1 << log2ceil(height), 1 << log2ceil(width));
		for (auto& v : vec_a) v = fft(v);
		vec_a = fft(vec_a);
		for (auto& v : vec_b) v = fft(v);
		vec_b = fft(vec_b);
		auto vec_c = vec_a;
		for (auto i = 0; i < vec_c.size(); ++i) {
			for (auto j = 0; j < vec_c[i].size(); ++j) {
				vec_c[i][j] *= vec_b[i][j];
			}
		}
		for (auto& v : vec_c) v = fft(v, true);
		vec_c = fft(vec_c, true);
		std::vector<std::vector<double>> result(height, std::vector<double>(width));
		for (auto i = 0; i < result.size(); ++i) {
			for (auto j = 0; j < result[i].size(); ++j) {
				result[i][j] = vec_c[i][j].real();
			}
		}
		return result;
	}
}