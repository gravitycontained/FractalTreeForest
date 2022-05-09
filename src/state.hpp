#pragma once
#include <qpl/qpl.hpp>


struct grow_state {
	qpl::f64 delta_angle = 0.1;
	qpl::size max_depth = 1u;
	qpl::size stop_depth = 1u;
	qpl::size min_depth_extra_branch = 3u;
	qpl::f64 exp_length = 0.8;
	qpl::f64 exp_thickness = 0.8;
	qpl::size branch_number = 2u;
	qpl::f64 initial_thickness = 4.0;
	qpl::f64 initial_length = 200.0;
	qpl::f64 branch_out_chance = 0.2;
	qpl::f64 branch_fractalness = 0.5;
	qpl::f64 delta_length = 0.0;
};

struct traits {
	qpl::f64 lightness = 0.0;
};

struct recursive_state {
	grow_state info;
	traits traits;
	qpl::vector2f starting_position;
	qpl::f64 angle;
	qpl::size depth;
};
