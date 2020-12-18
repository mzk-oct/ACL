#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <set>
#include <queue>
#include <string>
#include <map>
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
using namespace std;

constexpr int Solution_Count = 5;
constexpr int Vertex_Count = 225;
constexpr int Duration_Count = 20;
constexpr int Duration_Length = 50;
constexpr int Grid_Chage_Init = 25000;
constexpr int Grid_Charge_Max = 50000;
constexpr int EV_Charge_Init = 12500;
constexpr int EV_Charge_Max = 25000;
constexpr int Grid_Charge_Velocity = 800;
constexpr int EV_Charge_Velocity = 400;
constexpr int EV_Trans_Max = 4;
constexpr int EV_Move_Use = 50;
constexpr int Grid_Count = 20;
static int EV_Count;
constexpr int Time_Max = 1000;
constexpr int Time_Last = 900;
constexpr int Supply_Delta = 10;

namespace Input {
	struct GraphHeader {
		int vertex_count, edge_count;
	};
	struct Edge {
		int from, to, distance;
	};
	struct Edges {
		vector<Edge> edges;
	};
	enum class DayType {
		Sunny, SunnyWithRain, RainWithSunny, Rain
	};
	struct ForecastHeader {
		double event_probability;
		int segment_count, pattern_count, supply_delta2, event_delta;
	};
	struct SupplyForecast {
		vector<vector<int>> forecasts;
	};
	struct NanoGridHeader {
		int nano_grid, nano_grid_init, nano_grid_max, grid_velocity;
	};
	struct GridPattern {
		vector<pair<int, int>> pattern;
	};
	struct EVHeader {
		int ev_count, ev_init, ev_max, ev_velocity, trans_max, ev_move_delta;
	};
	struct EVPosition {
		vector<int> ev_pos;
	};
	struct TransInfo {
		double probability;
		int last_time;
	};
	struct Footer {
		int trans_penalty;
		double ele_penalty;
		int base_point_ele, base_point_trans;
	};

	istream& operator>>(istream& is, GraphHeader& header)
	{
		return is >> header.vertex_count >> header.edge_count;
	}

	istream& operator>>(istream& is, Edge& edge)
	{
		is >> edge.from >> edge.to >> edge.distance;
		--edge.from; --edge.to;
		return is;
	}

	istream& operator>>(istream& is, Edges& edges)
	{
		for (auto& e : edges.edges) {
			is >> e;
		}
		return is;
	}

	istream& operator>>(istream& is, DayType& type)
	{
		int t; is >> t;
		switch (t) {
		case 0: type = DayType::Sunny; break;
		case 1: type = DayType::SunnyWithRain; break;
		case 2: type = DayType::Rain; break;
		case 3: type = DayType::RainWithSunny; break;
		default: throw "Invalid DayType: " + to_string(t);
		}
		return is;
	}

	istream& operator>>(istream& is, ForecastHeader& header)
	{
		return is >> header.segment_count >> header.pattern_count >> header.supply_delta2 >> header.event_probability >> header.event_delta;
	}

	istream& operator>>(istream& is, SupplyForecast& forecast)
	{
		for (auto& line : forecast.forecasts) {
			for (auto& p : line) {
				is >> p;
			}
		}
		return is;
	}

	istream& operator>>(istream& is, NanoGridHeader& header)
	{
		return is >> header.nano_grid >> header.nano_grid_init >> header.nano_grid_max >> header.grid_velocity;
	}

	istream& operator>>(istream& is, GridPattern& pattern)
	{
		for (auto& t : pattern.pattern) {
			is >> t.first >> t.second; --t.first; --t.second;
		}
		return is;
	}

	istream& operator>>(istream& is, EVHeader& header)
	{
		return is >> header.ev_count >> header.ev_init >> header.ev_max >> header.ev_velocity >> header.trans_max >> header.ev_move_delta;
	}

	istream& operator>>(istream& is, EVPosition& ev_pos)
	{
		for (auto& p : ev_pos.ev_pos) {
			is >> p;
		}
		return is;
	}

	istream& operator>>(istream& is, TransInfo& info)
	{
		return is >> info.probability >> info.last_time;
	}

	istream& operator>>(istream& is, Footer& footer)
	{
		return is >> footer.trans_penalty >> footer.ele_penalty >> footer.base_point_ele >> footer.base_point_trans;
	}
	//input 2
	struct NanoGrid {
		int position, current, supplied, wasted, bought;
	};
	struct Grids {
		vector<NanoGrid> grids;
	};
	struct CarInfo {
		int charge;
		int u, v, from_u, from_v;
		vector<int> can_move_to;
		vector<int> in_stock;
	};
	struct CarInfos {
		vector<CarInfo> infos;
	};
	struct Order {
		int id, start, destination, state, time;
	};
	struct Orders {
		vector<Order> orders;
	};

	istream& operator>>(istream& is, NanoGrid& nano_grid) {
		return is >> nano_grid.position >> nano_grid.current >> nano_grid.supplied >> nano_grid.wasted >> nano_grid.bought;
	}

	istream& operator>>(istream& is, Grids& grids) {
		for (auto& g : grids.grids) {
			is >> g;
		}
		return is;
	}

	istream& operator>>(istream& is, CarInfo& info) {
		is >> info.charge;
		is >> info.u >> info.v >> info.from_u >> info.from_v; --info.u; --info.v;
		assert(info.u == info.v);
		assert(info.from_u == 0);
		assert(info.from_v == 0);
		int adj, ord; is >> adj;
		info.can_move_to.resize(adj);
		for (auto& a : info.can_move_to) {
			is >> a;
		}
		is >> ord;
		info.in_stock.resize(ord);
		for (auto& o : info.in_stock) {
			is >> o;
		}
		return is;
	}

	istream& operator>>(istream& is, CarInfos& infos) {
		for (auto& info : infos.infos) {
			is >> info;
		}
		return is;
	}

	istream& operator>>(istream& is, Order& o) {
		return is >> o.id >> o.start >> o.destination >> o.state >> o.time;
	}

	istream& operator>>(istream& is, Orders& orders) {
		int n; is >> n;
		orders.orders.resize(n);
		for (auto& o : orders.orders) {
			is >> o;
		}
		return is;
	}
}

struct Map {
	int vertex_count, edge_count;
	array<array<bool, Vertex_Count>, Vertex_Count> has_edge;
	array<array<int, Vertex_Count>, Vertex_Count> min_distance;
	array<int, Grid_Count> nano_grids;

};

void read_initials(istream& is, ostream* debug = nullptr) {
	using namespace Input;
	int solution_count; is >> solution_count;
	GraphHeader graph_header;
	is >> graph_header;
	Edges edges{ vector<Edge>(graph_header.edge_count) };
	is >> edges;
	DayType day_type;
	is >> day_type;
	ForecastHeader forecast_header;
	is >> forecast_header;
	cerr << forecast_header.segment_count << ' ' << forecast_header.pattern_count << ' ' << forecast_header.supply_delta2 << ' ' << forecast_header.event_probability << ' ' << forecast_header.event_delta << '\n';
	assert(forecast_header.segment_count == Duration_Count);
	assert(forecast_header.supply_delta2 == 100);
	SupplyForecast forecast{ vector<vector<int>>(3, vector<int>(forecast_header.segment_count)) };
	is >> forecast;
	NanoGridHeader nano_grid_header;
	is >> nano_grid_header;
	assert(nano_grid_header.nano_grid == Grid_Count);
	GridPattern pattern{ vector<pair<int, int>>(nano_grid_header.nano_grid) };
	is >> pattern;
	EVHeader ev_header;
	is >> ev_header;
	assert(ev_header.ev_max == EV_Charge_Max);
	EV_Count = ev_header.ev_count;
	EVPosition ev_pos{ vector<int>(ev_header.ev_count) };
	is >> ev_pos;
	TransInfo info;
	is >> info;
	Footer footer;
	is >> footer;
	int t_max;
	is >> t_max;
	cerr << t_max << '\n';
	assert(t_max == Time_Max);
	if (debug != nullptr) {
		int sum_supply{ 0 };
		for (const auto [v, pat] : pattern.pattern) {
			if (pat == -1) continue;
			sum_supply += accumulate(forecast.forecasts[pat].begin(), forecast.forecasts[pat].end(), 0);
		}
		*debug << "sum supply: " << sum_supply << '\n';
	}
}

void read_period(istream& is, ostream* debug = nullptr) {
	using namespace Input;
	Grids grids{ vector<NanoGrid>(Grid_Count) };
	is >> grids;
	CarInfos infos{ vector<CarInfo>(EV_Count) };
	is >> infos;
	Orders orders;
	is >> orders;
}

void show_stay(ostream& os, ostream* debug = nullptr) {
	if (debug != nullptr) {
		*debug << "start show\n";
	}
	for (auto i = 0; i < EV_Count; ++i) {
		os << "stay\n";
	}
	os << flush;
	if (debug != nullptr) {
		*debug << "start end\n";
	}
}


void read_score(istream& is, ostream* debug = nullptr) {
	double trans, ele;
	is >> trans >> ele;
	if (debug != nullptr) {
		*debug << "trans: " << setprecision(10) << fixed << trans << ", ele: " << setprecision(10) << fixed << ele << '\n';
	}

}

int main2() {
	//fstream debug_out("/mnt/c/Documents\\ and\\ Settings/yudedako/Desktop/テスター/toolkit_B/debug.txt");
	cin.tie(nullptr)->sync_with_stdio(false);
	read_initials(cin, &cerr);
	for (auto i = 0; i < Solution_Count; ++i) {
		cerr << "Solution Count: " << i << endl;
		for (auto t = 0; t < Time_Max; ++t) {
			read_period(cin);
			show_stay(cout, &cerr);
		}
		read_period(cin);
		read_score(cin, &cerr);
	}
}
}