#pragma once
#include <qpl/qpl.hpp>


namespace va {
	qsf::vertex_array va;

	void init() {
		va.set_primitive_type(sf::Quads);
	}
	void clear() {
		va.clear();
	}
}

namespace color {
	constexpr auto brown = qpl::rgb(100, 50, 0);
	constexpr auto green = qpl::rgb(50, 175, 60);
	constexpr auto dark_green = qpl::rgb(20, 150, 60);

	std::vector<qpl::rgb> interpolate = { brown, brown, green, dark_green };
}

namespace random {
	qpl::random_engine<64u> engine;
	qpl::u64 engine_seed = 0u;

	void reset() {
		engine.seed(engine_seed);
	}
	void seed_random() {
		engine_seed = qpl::random();
		reset();
	}
}