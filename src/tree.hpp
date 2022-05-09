#pragma once
#include <qpl/qpl.hpp>
#include "segment.hpp"
#include "data.hpp"


struct tree {
	std::vector<tree> trees;
	segment segment;

	void grow_recursive(recursive_state& state) {
		
		this->segment.create(state);

		if (state.depth >= state.info.stop_depth) {
			return;
		}
		this->trees.resize(state.info.branch_number);
		for (qpl::size i = 0u; i < state.info.branch_number; ++i) {

			auto progress = qpl::f64_cast(i) / (state.info.branch_number - 1);
			auto branch_angle_fractal = (progress * state.info.delta_angle * 2) - state.info.delta_angle;
			auto branch_angle_random = random::engine.generate(-state.info.delta_angle, state.info.delta_angle);

			auto branch_angle = qpl::linear_interpolation(branch_angle_fractal, branch_angle_random, 1 - state.info.branch_fractalness);
			
			auto pos_before = state.starting_position;
			state.starting_position = this->segment.line.b;
			state.angle += branch_angle;
			++state.depth;

			this->trees[i].grow_recursive(state);

			--state.depth;
			state.angle -= branch_angle;
			state.starting_position = pos_before;
		}
		if (state.depth >= state.info.min_depth_extra_branch && random::engine.generate_b(state.info.branch_out_chance)) {
			this->trees.push_back({});

			qpl::vector2f new_position;
			new_position = (this->segment.line.b - this->segment.line.a);
			new_position *= random::engine.generate_0_1();
			new_position += this->segment.line.a;

			qpl::f64 branch_angle = state.info.delta_angle * random::engine.generate(0.8, 1.25);
			if (random::engine.generate_b()) {
				branch_angle *= -1;
			}

			auto stop_depth_before = state.info.stop_depth;
			state.info.stop_depth = qpl::min(state.info.stop_depth, state.depth + 4);
			auto pos_before = state.starting_position;
			state.starting_position = new_position;
			state.angle += branch_angle;
			++state.depth;

			this->trees.back().grow_recursive(state);

			--state.depth;
			state.angle -= branch_angle;
			state.starting_position = pos_before;
			state.info.stop_depth = stop_depth_before;
		}
	}
};

struct tree_builder {

	grow_state state;
	traits traits;
	tree tree;

	void grow(qpl::vector2f position) {
		auto state_copy = this->state;
		recursive_state recursive_state;
		recursive_state.info = this->state;
		recursive_state.traits = this->traits;
		recursive_state.info.stop_depth = recursive_state.info.max_depth;
		recursive_state.angle = -qpl::pi / 2;
		recursive_state.depth = 0u;
		recursive_state.starting_position = position;
		this->tree.trees.clear();
		this->tree.grow_recursive(recursive_state);
	}
};
