#pragma once
#include <qpl/qpl.hpp>
#include "state.hpp"


struct segment {
	qpl::straight_line line;

	void make_shape(qpl::vector2f pos1, qpl::vector2f pos2, qpl::f64 thickness1, qpl::f64 thickness2, qpl::rgb color1, qpl::rgb color2) {
		this->line.a = pos1;
		this->line.b = pos2;

		auto normal = this->line.normal();

		auto size = va::va.size();
		va::va.resize(size + 4u);

		va::va[size + 0].position = pos1 - normal * thickness1;
		va::va[size + 1].position = pos1 + normal * thickness1;
		va::va[size + 2].position = pos2 + normal * thickness2;
		va::va[size + 3].position = pos2 - normal * thickness2;

		va::va[size + 0].color = color1;
		va::va[size + 1].color = color1;
		va::va[size + 2].color = color2;
		va::va[size + 3].color = color2;

	}

	void create(recursive_state& state) {
		auto length = std::pow(state.info.exp_length, state.depth) * state.info.initial_length;

		length *= (random::engine.generate(-state.info.delta_length, state.info.delta_length) + 1);

		if (state.depth == 0u) {
			length *= 2;
		}
		auto thickness1 = std::pow(state.info.exp_thickness, state.depth) * state.info.initial_thickness;
		auto thickness2 = std::pow(state.info.exp_thickness, state.depth + 1) * state.info.initial_thickness;

		auto angle_pos = qpl::vec(std::cos(state.angle), std::sin(state.angle));

		auto pos1 = state.starting_position;
		auto pos2 = state.starting_position + angle_pos * length;

		auto progress1 = qpl::f64_cast(state.depth) / state.info.max_depth;
		auto progress2 = qpl::f64_cast(state.depth + 1) / state.info.max_depth;

		qpl::rgb color1 = qpl::rgb::interpolation(color::interpolate, progress1);
		qpl::rgb color2 = qpl::rgb::interpolation(color::interpolate, progress2);


		if (state.traits.lightness < 0.0) {
			color1.interpolate(qpl::rgb::black, -state.traits.lightness);
			color2.interpolate(qpl::rgb::black, -state.traits.lightness);
		}
		else {
			color1.interpolate(qpl::rgb::white, state.traits.lightness / 2);
			color2.interpolate(qpl::rgb::white, state.traits.lightness / 2);
		}
		if (state.depth == state.info.stop_depth) {
			thickness2 = 0;
		}

		this->make_shape(pos1, pos2, thickness1, thickness2, color1, color2);

	}
};