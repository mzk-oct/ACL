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
#include <cfloat>
//#define NDEBUG;
#include <cassert>
using namespace std;
ostream* logstream = &cerr;

void writeln() {
	(*logstream) << '\n';
}
template<typename Head, typename... Tail>
void writeln(Head&& head, Tail&&... tail) {
	*logstream << std::forward<Head>(head);
	writeln(std::forward<Tail>(tail)...);
}

#define DLEVEL 5

#if (DLEVEL > 4)
#define verb(...) writeln("verb: ", __VA_ARGS__);
#define verb_if(expr, ...) if (expr) verb(__VA_ARGS__);
#else
#define verb(...)
#define verb_if(...);
#endif
#if (DLEVEL > 3)
#define info(...) writeln("info: ", __VA_ARGS__);
#define info_if(expr, ...) if (expr) info(__VA_ARGS__);
#else
#define info(...)
#define info_if(...) if (expr) info(__VA_ARGS__);
#endif
#if (DLEVEL > 2)
#define log(...) writeln("log :", __VA_ARGS__);
#define log_if(expr, ...) if (expr) log(__VA_ARGS__);
#else
#define log(...)
#define log_if(...)
#endif

#define STATICS

using Vertex = int;
using Distance = int;
using Probability = double;
using ChargeDiff = double;
using Percentage = unsigned int;

constexpr int Solution_Count = 5;
constexpr int Vertex_Count = 225;
constexpr int Duration_Count = 20;
constexpr int Predict_Delta2 = 100;
constexpr int Duration_Length = 50;
constexpr int Event_Delta = 1000;
constexpr int Grid_Charge_Init = 25'000;
constexpr int Grid_Charge_Min = 0;
constexpr int Grid_Charge_Max = 50'000;
constexpr int EV_Charge_Min = 12'500;
constexpr int EV_Charge_Init = 12'500;
constexpr int EV_Charge_Max = 2'5000;
constexpr int Grid_Charge_Velocity = 800;
constexpr int EV_Charge_Velocity = 400;
constexpr int EV_Trans_Max = 4;
constexpr int EV_Move_Delta = 50;
constexpr int Grid_Count = 20;
static int EV_Count;
constexpr int Time_Max = 1000;
constexpr int Time_Last = 900;
constexpr int Supply_Delta = 10;
constexpr double Trans_Probability = 0.2;
constexpr int Power_Supply_Pattern = 3;
constexpr long long int Base_Ele_Point = -1'500'000;
constexpr long long int Base_Trans_Point = -1'900'000;


struct IntRange {
	int from, until;
	struct Iter {
		int current;
		Iter& operator++();
		const int* operator->() const;
		const int& operator*() const;
		bool operator!=(const Iter& other) const;
	};
	Iter begin() const;
	Iter end() const;
	bool contains(int value) const;
};
IntRange range(const int from, const int until);
IntRange range(const int until);

struct DoubleRange {
	double from, to;
};
struct StandardNormalDistribution {
	static constexpr std::array<double, 101> from_percentage{ 0,0.012533469508069,0.025068908258711,0.037608287661256,0.050153583464734,0.062706777943214,0.07526986209983,0.087844837895872,0.10043372051147,0.113038540644565,0.125661346855074,0.138304207961405,0.150969215496777,0.163658486233141,0.176374164780861,0.189118426272792,0.201893479141851,0.214701568001744,0.227544976641149,0.240426031142308,0.2533471031358,0.266310613204095,0.279319034447454,0.292374896226804,0.305480788099397,0.318639363964375,0.331853346436817,0.345125531470472,0.358458793251194,0.371856089385075,0.385320466407568,0.398855065642337,0.412463129441405,0.426148007841278,0.439913165673234,0.45376219016988,0.467698799114508,0.48172684958473,0.495850347347453,0.510073456968595,0.524400512708041,0.53883603027845,0.553384719555673,0.568051498338983,0.582841507271216,0.597760126042478,0.612812991016627,0.62800601443757,0.643345405392917,0.658837692736188,0.674489750196082,0.690308823933034,0.706302562840088,0.722479051928063,0.738846849185214,0.755415026360469,0.772193214188685,0.789191652658222,0.80642124701824,0.823893630338557,0.841621233572914,0.859617364241911,0.877896295051229,0.896473364001916,0.915365087842814,0.93458929107348,0.954165253146195,0.974113877059309,0.994457883209753,1.01522203321703,1.03643338949379,1.05812161768478,1.08031934081496,1.1030625561996,1.1263911290388,1.15034938037601,1.17498679206609,1.20035885803086,1.22652812003661,1.25356543847045,1.2815515655446,1.31057911216813,1.34075503369022,1.37220380899873,1.40507156030963,1.43953147093846,1.47579102817917,1.51410188761928,1.55477359459685,1.59819313992282,1.64485362695147,1.69539771027214,1.75068607125217,1.8119106729526,1.88079360815125,1.95996398454006,2.05374891063182,2.17009037758456,2.32634787404084,2.5758293035489, 3.0 };
	static double upper_bound(Percentage percent) {
		assert(0 <= percent && percent <= 100);
		if (percent >= 50) {
			return from_percentage[(percent - 50) << 1];
		}
		else {
			return -from_percentage[(50 - percent) << 1];
		}
	}
	static double upper_bound(Percentage percent, double average, double deviation) {
		return average + upper_bound(percent) * deviation;
	}
	static double lower_bound(Percentage percent) {
		assert(0 <= percent && percent <= 100);
		if (percent >= 50) {
			return -from_percentage[(percent - 50) << 1];
		}
		else {
			return from_percentage[(50 - percent) << 1];
		}
	}
	static double lower_bound(Percentage percent, double average, double deviation) {
		return average + lower_bound(percent);
	}
	static DoubleRange range(Percentage percent) {
		assert(0 <= percent && percent <= 100);
		return DoubleRange{ -from_percentage[percent], from_percentage[percent] };
	}
	static DoubleRange range(Percentage percent, double average, double deviation) {
		assert(0 <= percent && percent <= 100);
		return DoubleRange{ average - from_percentage[percent] * deviation, average + from_percentage[percent] * deviation };
	}
};



namespace Input {
	struct GridData {
		struct GridType {
			int id, type;
		};
		int day_type{ 0 };
		Probability event_probability{ 0 };
		array<array<int, Duration_Count>, Power_Supply_Pattern> predict{};
		array<GridType, Grid_Count> grid_type{};
		static void read_grid_data(istream& is);
	};
	struct GraphData {
		struct Edge {
			int to, distance;
		};
		int edge_count{ 0 };
		array<array<int, Vertex_Count>, Vertex_Count> min_distance{ array<int, Vertex_Count>{INT_MAX >> 1} };
		array<vector<Edge>, Vertex_Count> edges;
		static void read_graph_data(istream& is);
	};
	struct EVData {
		int ev_count{ 0 };
		vector<int> initial_ev_position;
		static void read_ev_data(istream& is);
	};
	void read_initial_state(istream& is);
	void read_update(istream& is);
	void read_score(istream& is);
}
namespace Output {
	struct Command {
		virtual ~Command() = 0;
	};
	void execute_command(ostream &os);
}
namespace Control {
	void setup(istream& is);
	void update(istream& is);
	void calculate_next_strategy();
	void execute(ostream& os);
	void solution_score(istream& is);
}


static Input::GridData grid_data;
static Input::GraphData graph_data;
static Input::EVData ev_data;
static int Current_Time;


int main() {
	istream& is = cin;
	ostream& os = cout;
	(*logstream) << setprecision(1) << fixed;
	Control::setup(is);
	for (auto solution = 0; solution < Solution_Count; ++solution) {
		log("Solution", solution, ", Start");
		for (Current_Time = 0; Current_Time < Time_Max; ++Current_Time) {
			log_if(Current_Time % Duration_Length == 0, "Duration: ", Current_Time / Duration_Length, ", Start");
			Control::update(is);
			Control::calculate_next_strategy();
			Control::execute(os);
		}
		Control::update(is);
		Control::solution_score(is);
	}
}




void distance_statics() {
	log("Statics: Distance");
	vector<int> all_pair;
	for (auto i = 0; i < Vertex_Count; ++i) {
		for (auto j = 0; j < i; ++j) {
			all_pair.push_back(graph_data.min_distance[i][j]);
		}
	}
	sort(all_pair.begin(), all_pair.end());
	const auto sum = accumulate(all_pair.begin(), all_pair.end(), 0);
	const auto average = double(sum) / all_pair.size();
	constexpr int all = 25'200;
	assert(all == all_pair.size());
	for (auto i = 0; i < 10; ++i) {
		int partial_sum{ 0 };
		for (auto j = 0; j < all / 10; ++j) {
			partial_sum += all_pair[i * all / 10 + j];
		}
		double partial_averge = double(partial_sum) / (all / 10);
		log("[", i * 10, "%, ", (i + 1) * 10, "%] •½‹Ï: ", partial_averge);
	}
	log("‘S‘Ì•½‹Ï: ", average);
}
void power_statics() {
	log("Statics: Power Predict");
	IntRange lack{ INT_MIN, 0 }, crisis{ 0, 1'000 }, serious{ 1'000, 5'000 }, low{ 5'000, 12'500 }, normal{ 12'500, 500 }, high{ 37'500, 45'000 }, fill{ 45'000, 50'001 }, overflow{ 50'001, INT_MAX };
	array<pair<string, IntRange>, 8> labels{pair(" lack   ", lack), pair(" crisis ", crisis), pair(" serious", serious), pair(" low    ", low), pair(" normal ", normal), pair(" high   ", high), pair(" fill   ", fill), pair("overflow", overflow)};
	log("label definition");
	for (const auto &[label, range] : labels) {
		log(label, ": [", range.from, ", ", range.until, ")");
	}

	array<int, Grid_Count> grid_power{ Grid_Charge_Init };
	array<array<int, 8>, Power_Supply_Pattern> summary{ array<int, 8>{0} };
	array<int, 3> buy_all{ 0 }, wasted_all{ 0 };

	for (auto duration = 0; duration < Duration_Count; ++duration) {
		array<int, Power_Supply_Pattern> buy{ 0 }, wasted{ 0 };
		array<array<int, 8>, Power_Supply_Pattern> count{ array<int, 8>{0} };
		for (auto time = duration * Duration_Length; time < (duration + 1) * Duration_Length; ++time) {
			for (auto g = 0; g < Grid_Count; ++g) {
				grid_power[g] += grid_data.predict[grid_data.grid_type[g].type][time / Duration_Length];
				for (auto i = 0; i < labels.size(); ++i) {
					if (labels[i].second.contains(grid_power[g])) {
						count[grid_data.grid_type[g].type][i] += 1;
					}
				}
				if (grid_power[g] > Grid_Charge_Max) {
					wasted[grid_data.grid_type[g].type] += grid_power[g] - Grid_Charge_Max;
				}
				if (grid_power[g] < Grid_Charge_Min) {
					buy[grid_data.grid_type[g].type] += Grid_Charge_Min - grid_power[g];
				}
				grid_power[g] = clamp(grid_power[g], Grid_Charge_Min, Grid_Charge_Max);
			}
		}
		log("In Duration: ", duration);
		for (auto i = 0; i < labels.size(); ++i) {
			for (auto type = 0; type < Power_Supply_Pattern; ++type) {
				summary[type][i] += count[type][i];
				log(labels[i].first, ": ", "type ", type, ": ", count[type][i]);
			}
		}
		for (auto type = 0; type < Power_Supply_Pattern; ++type) {
			log("  Buy   : ", "type ", type, ": ", buy[type]);
			buy_all[type] += buy[type];
		}
		for (auto type = 0; type < Power_Supply_Pattern; ++type) {
			log(" Wasted : ", "type ", type, ": ", wasted[type]);
			wasted_all[type] += wasted[type];
		}
	}
	log("Summary");
	for (auto i = 0; i < labels.size(); ++i) {
		for (auto type = 0; type < Power_Supply_Pattern; ++type) {
			log(labels[i].first, ": ", "type ", type, ": ", summary[type][i]);
		}
	}
	for (auto type = 0; type < Power_Supply_Pattern; ++type) {
		log("  Buy   : ", "type ", type, ": ", buy_all[type]);
	}
	for (auto type = 0; type < Power_Supply_Pattern; ++type) {
		log(" Wasted : ", "type ", type, ": ", wasted_all[type]);
	}
}
void show_initial_statics() {
#ifdef STATICS
	distance_statics();
	power_statics();
#endif 
}






//impl

void Input::GridData::read_grid_data(istream& is)
{
	int pass;
	is >> grid_data.day_type;
	is >> pass >> pass >> pass >> grid_data.event_probability >> pass;
	assert(pass == Event_Delta);
	for (auto& line : grid_data.predict) {
		for (auto& p : line) {
			is >> p;
		}
	}
	is >> pass >> pass >> pass >> pass;
	assert(pass == Grid_Charge_Velocity);
	for (auto& [x, pattern] : grid_data.grid_type) {
		is >> x >> pattern;
		--x; --pattern;
	}
}
void Input::GraphData::read_graph_data(istream& is) {
	int pass;
	is >> pass >> graph_data.edge_count;
	assert(pass == Vertex_Count);
	for (auto _i = 0; _i < graph_data.edge_count; ++_i) {
		int u, v, d; is >> u >> v >> d; --u; --v;
		graph_data.edges[u].push_back(Edge{ v, d });
		graph_data.edges[v].push_back(Edge{ u, d });
		graph_data.min_distance[u][v] = graph_data.min_distance[v][u] = d;
	}
	for (auto k = 0; k < Vertex_Count; ++k) {
		for (auto i = 0; i < Vertex_Count; ++i) {
			for (auto j = 0; j < Vertex_Count; ++j) {
				graph_data.min_distance[i][j] = std::min(graph_data.min_distance[i][j], graph_data.min_distance[i][k] + graph_data.min_distance[k][j]);
			}
		}
	}

}
void Input::EVData::read_ev_data(istream& is) {
	int pass;
	is >> ev_data.ev_count >> pass >> pass >> pass >> pass >> pass;
	assert(pass == EV_Move_Delta);
	ev_data.initial_ev_position.resize(ev_data.ev_count);
	for (auto& pos : ev_data.initial_ev_position) {
		is >> pos; --pos;
	}
	is >> pass >> pass;
	assert(pass == Time_Last);
}
void Input::read_initial_state(istream& is) {
	int pass; is >> pass;
	assert(pass == Solution_Count);
	GridData::read_grid_data(is);
	GraphData::read_graph_data(is);
	EVData::read_ev_data(is);
	is >> pass >> pass >> pass >> pass;
	assert(pass == Base_Trans_Point);
	is >> pass;
	assert(pass == Time_Max);
}
void Control::setup(istream& is) {
	Input::read_initial_state(is);
	show_initial_statics();
}