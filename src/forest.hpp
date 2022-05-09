#pragma once
#include <qpl/qpl.hpp>

#include "tree.hpp"



struct forest {
	std::vector<tree_builder> trees;
	qpl::size tree_count = 50u;

	void grow(qpl::vector2f dimension) {
		va::clear();
		this->trees.resize(this->tree_count);

		qpl::vector2f position;
		position.x = dimension.x / 2;
		position.y = dimension.y;

		for (qpl::size i = 0u; i < this->trees.size(); ++i) {
			position.x = random::engine.generate(-1500.0, dimension.x + 1500.0);

			qpl::f64 z = 0.0;
			if (this->trees.size() > 1) {
				z = (qpl::f64_cast(i) / (this->trees.size()));
			}
			auto z_neg1_pos1 = (z - 0.5) * 2;

			auto thickness_z = std::pow(1 - z * 0.8, 2.0) * 5;
			auto length_z = std::pow(1 - z * 0.6, 2.0) * 3;


			if (this->trees.size() == 1) {
				thickness_z = length_z = 1.0;
				z_neg1_pos1 = 0.0;
			}

			auto& tree = this->trees[i];

			tree.state.initial_thickness = random::engine.generate(2.0, 5.0) * thickness_z;
			tree.state.initial_length = random::engine.generate(100.0, 200.0) * length_z;
			tree.traits.lightness = z_neg1_pos1 * 0.7;

			qpl::i32 depth = tree.state.max_depth;
			depth = qpl::i32_cast(depth * (1 - z / 2));
			
			tree.state.max_depth = depth;

			tree.grow(position);
		}
	}

	void draw(qsf::draw_object& draw) const {
		draw.draw(va::va);
	}
};