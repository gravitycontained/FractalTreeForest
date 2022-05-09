#include <qpl/qpl.hpp>
#include "data.hpp"
#include "segment.hpp"
#include "forest.hpp"
#include "background.hpp"

namespace config {
	constexpr bool use_branch_numbers = true;
}

struct main_state : qsf::base_state {

	void create_slider(auto& slider, auto min, auto max, auto value, qpl::u32& ctr, std::string description) {

		constexpr auto slider_dimension = qpl::vec(400, 20);
		constexpr auto knob_dimension = qpl::vec(20, 20);

		auto pos = qpl::vec(this->dimension().x - slider_dimension.x - 20 , (ctr) * 30 + 20);

		if (ctr == 0u) {
			this->hud_hitbox.position = pos;
		}
		this->hud_hitbox.dimension = (pos + slider_dimension) - this->hud_hitbox.position;

		slider.set_dimensions(slider_dimension, knob_dimension);
		slider.set_position(pos);
		slider.set_range(min, max, value);

		slider.set_hover_increase(2.0);

		this->slider_info_text.push_back({});
		auto& text = this->slider_info_text.back();
		text.set_font("arial");
		text.set_character_size(18u);
		text.set_string(description, " : ");

		qpl::vector2f text_position;
		text_position.x = pos.x - text.get_visible_hitbox().dimension.x;
		text_position.y = (pos.y + (slider_dimension.y / 2 - 5)) - text.get_visible_hitbox().dimension.y / 2;
		text.set_position(text_position);

		this->max_slider_text_width = qpl::max(this->max_slider_text_width, text.get_visible_hitbox().dimension.x);

		++ctr;
	}

	void init_sliders() {

		this->slider_info_text.clear();
		qpl::u32 ctr = 0u;
		this->create_slider(this->slider_angle, 0.0, qpl::pi, 0.5, ctr, "angle");
		this->create_slider(this->slider_depth, 1u, 25u, 7u, ctr, "max depth");
		this->create_slider(this->slider_exp_length, 0.0, 1.2, 0.8, ctr, "exp length");
		this->create_slider(this->slider_exp_thickness, 0.0, 1.2, 0.8, ctr, "exp thickness");
		if constexpr (config::use_branch_numbers) {
			this->create_slider(this->slider_branch_size, 1u, 5u, 2u, ctr, "branches");
		}
		this->create_slider(this->slider_branch_out_chance, 0.0, 1.0, 0.4, ctr, "branch chance");
		this->create_slider(this->slider_fractalness, 0.0, 1.0, 0.6, ctr, "fractalness");
		this->create_slider(this->slider_delta_length, 0.0, 1.0, 0.3, ctr, "delta length");
		this->create_slider(this->slider_tree_count, 1u, 200u, 50u, ctr, "tree count");

		this->hud_hitbox.position.move(qpl::vec(-this->max_slider_text_width, 0));
		this->hud_hitbox.dimension.move(qpl::vec(this->max_slider_text_width, 0));
		this->hud_hitbox.increase(10);

		this->hud_background.set_hitbox(this->hud_hitbox);
		this->hud_background.set_color(qpl::rgb::black);

		this->hud_hitbox.increase(40);
	}

	void call_on_resize() override {
		this->forest.grow(this->dimension());
		this->background.init(this->dimension());

		this->init_sliders();
		this->view.set_hitbox(*this);

		this->hover_hud_animation.go_forwards();
		this->hover_hud_animation.set_duration(0.3);
	}

	void init() override {
		this->seed_button.set_text_font("arial");
		this->seed_button.set_text_string("randomize");
		this->seed_button.set_text_character_size(22u);
		this->seed_button.set_position(qpl::vec(10, 10));
		this->seed_button.set_dimension(qpl::vec(130, 40));
		this->seed_button.set_text_color(qpl::rgb::white);
		this->seed_button.centerize_text();

		this->seed_button_animation.set_duration(0.1);
		this->seed_button_click_animation.set_duration(0.1);

		this->vertices_text.set_font("arial");
		this->vertices_text.set_character_size(22u);
		this->vertices_text.set_position(qpl::vec(10, 60));
		

		this->call_on_resize();

		this->view.set_scale(5);
		this->view.set_position(qpl::vec(-2300, -3000));
	}

	void update_slider(auto& slider, auto& value, bool& changed) {
		this->update(slider);
		if (slider.value_changed) {
			value = slider.get_value();
			changed = true;
		}
		if (slider.dragging) {
			this->any_slider_dragging = true;
		}
	}

	void update_sliders() {
		bool changed = false;
		this->any_slider_dragging = false;

		if (!this->display_hud) {
			return;
		}

		grow_state state;
		state = this->forest.trees.front().state;
		this->update_slider(this->slider_angle, state.delta_angle, changed);
		this->update_slider(this->slider_depth, state.max_depth, changed);
		this->update_slider(this->slider_exp_length, state.exp_length, changed);
		this->update_slider(this->slider_exp_thickness, state.exp_thickness, changed);
		if constexpr (config::use_branch_numbers) {
			this->update_slider(this->slider_branch_size, state.branch_number, changed);
		}
		this->update_slider(this->slider_branch_out_chance, state.branch_out_chance, changed);
		this->update_slider(this->slider_fractalness, state.branch_fractalness, changed);
		this->update_slider(this->slider_delta_length, state.delta_length, changed);
		this->update_slider(this->slider_tree_count, this->forest.tree_count, changed);

		for (auto& tree : this->forest.trees) {
			tree.state = state;
		}

		if (changed) {
			random::reset();
			this->forest.grow(this->dimension());
			this->vertices_text.set_string("vertices: ", qpl::big_number_string(va::va.vertices.size()));
		}
	}

	void update_hud_hover() {

		auto hovering = this->hud_hitbox.contains(this->event().mouse_position());
		if (hovering && !this->hud_hovering_before) {
			this->hover_hud_animation.go_forwards();
			this->hover_hud_animation.set_duration(0.3);
		}
		else if (!hovering && this->hud_hovering_before) {
			this->hover_hud_animation.go_backwards();
			this->hover_hud_animation.set_duration(0.5);
		}

		this->hover_hud_animation.update();
		if (this->hover_hud_animation.is_running()) {
			auto offset = 0.4;
			auto progress = this->hover_hud_animation.get_curve_progress() * (1 - offset) + offset;
			auto alpha = qpl::u8_cast(progress * 255);

			this->slider_angle.set_alpha(alpha);
			this->slider_depth.set_alpha(alpha);
			this->slider_exp_length.set_alpha(alpha);
			this->slider_exp_thickness.set_alpha(alpha);
			this->slider_branch_size.set_alpha(alpha);
			this->slider_branch_out_chance.set_alpha(alpha);
			this->slider_fractalness.set_alpha(alpha);
			this->slider_delta_length.set_alpha(alpha);
			this->slider_tree_count.set_alpha(alpha);
			this->hud_background.set_multiplied_alpha(alpha);

			for (auto& text : this->slider_info_text) {
				text.set_multiplied_alpha(alpha);
			}
		}

		this->hud_hovering_before = hovering;
	}

	void update_seed_button() {
		if (this->display_hud) {
			this->update(this->seed_button);
			if (this->seed_button.is_clicked()) {
				random::seed_random();
				this->forest.grow(this->dimension());
				this->seed_button_click_animation.set_progress(1.0, true);
			}

			auto hovering = this->seed_button.is_hovering();
			if (hovering && !this->seed_button_hovering_before) {
				this->seed_button_animation.go_forwards();
			}
			else if (!hovering && this->seed_button_hovering_before) {
				this->seed_button_animation.go_backwards();
			}

			qpl::rgb text_color = qpl::rgb::white;

			this->seed_button_animation.update();
			if (this->seed_button_animation.is_running()) {
				auto progress = this->seed_button_animation.get_curve_progress();
				auto background_color = qpl::rgb::black.interpolated(qpl::rgb(50, 50, 50), progress);
				text_color = qpl::rgb::white.interpolated(qpl::rgb::green, progress / 2);
				this->seed_button.set_background_color(background_color);
				this->seed_button.set_text_color(text_color);
			}
			if (this->seed_button_animation.get_progress() == 0.0) {
				text_color = qpl::rgb::white;
			}
			else if (this->seed_button_animation.get_progress() >= 1.0) {
				text_color = qpl::rgb::white.interpolated(qpl::rgb::green, 0.5);
			}

			this->seed_button_click_animation.update();
			if (this->seed_button_click_animation.is_running()) {
				auto progress = this->seed_button_click_animation.get_curve_progress();
				text_color = text_color.interpolated(qpl::rgb::cyan, progress);
				this->seed_button.set_text_color(text_color);
			}
			this->seed_button_hovering_before = hovering;
		}
	}

	void updating() override {
		if (this->event().key_single_pressed(sf::Keyboard::H)) {
			this->display_hud = !this->display_hud;
		}

		this->update_sliders();

		this->view.allow_dragging = !this->any_slider_dragging;
		this->update(this->view);

		this->update_hud_hover();
		this->update_seed_button();
	}

	void drawing() override {
		this->draw(this->background.background_rect);
		this->draw(this->background.grass, this->view);

		this->draw(this->forest, this->view);

		if (this->display_hud) {
			this->draw(this->hud_background);

			this->draw(this->slider_angle);
			this->draw(this->slider_depth);
			this->draw(this->slider_exp_length);
			this->draw(this->slider_exp_thickness);
			if constexpr (config::use_branch_numbers) {
				this->draw(this->slider_branch_size);
			}
			this->draw(this->slider_branch_out_chance);
			this->draw(this->slider_fractalness);
			this->draw(this->slider_delta_length);
			this->draw(this->slider_info_text);
			this->draw(this->slider_tree_count);


			this->draw(this->vertices_text);
			this->draw(this->seed_button);
		}
	}

	forest forest;
	qsf::view_rectangle view;
	qsf::slider<qpl::f64> slider_angle;
	qsf::slider<qpl::size> slider_depth;
	qsf::slider<qpl::f64> slider_exp_length;
	qsf::slider<qpl::f64> slider_exp_thickness;
	qsf::slider<qpl::size> slider_branch_size;
	qsf::slider<qpl::f64> slider_branch_out_chance;
	qsf::slider<qpl::f64> slider_fractalness;
	qsf::slider<qpl::f64> slider_delta_length;
	qsf::slider<qpl::size> slider_tree_count;

	std::vector<qsf::text> slider_info_text;
	qpl::f32 max_slider_text_width = 0.0;

	qsf::smooth_button seed_button;
	qpl::animation seed_button_animation;
	qpl::animation seed_button_click_animation;
	bool seed_button_hovering_before = false;

	background background;

	qpl::animation hover_hud_animation;
	qpl::hitbox hud_hitbox;
	qsf::smooth_rectangle hud_background;
	bool hud_hovering_before = false;
	bool display_hud = true;

	qsf::text vertices_text;
	bool any_slider_dragging = false;
};

int main() try {
	va::init();
	random::seed_random();

	qsf::framework framework;
	framework.set_title("QPL");
	framework.add_font("arial", "resources/arial.ttf");
	framework.set_dimension({ 1400u, 950u });

	framework.add_state<main_state>();
	framework.game_loop();
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}