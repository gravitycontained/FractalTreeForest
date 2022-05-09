#pragma once
#include <qpl/qpl.hpp>


struct background {
	qsf::vertex_array background_rect;
	qsf::vertex_array grass;

	void init(qpl::vector2f dimension) {

		auto N = 10'000;

		this->grass.set_primitive_type(sf::Quads);
		this->grass.resize(4u);
		this->grass[0].position = qpl::vec(-N, dimension.y);
		this->grass[1].position = qpl::vec(dimension.x + N, dimension.y);
		this->grass[2].position = qpl::vec(dimension.x + N, dimension.y + N / 2);
		this->grass[3].position = qpl::vec(-N, dimension.y + N / 2);

		this->grass[0].color = qpl::rgb(0, 40, 20);
		this->grass[1].color = qpl::rgb(0, 40, 20);
		this->grass[2].color = qpl::rgb(0, 0, 0);
		this->grass[3].color = qpl::rgb(0, 0, 0);



		this->background_rect.set_primitive_type(sf::Quads);
		this->background_rect.resize(4u);
		this->background_rect[0].position = qpl::vec(0, 0);
		this->background_rect[1].position = qpl::vec(dimension.x, 0);
		this->background_rect[2].position = qpl::vec(dimension.x, dimension.y);
		this->background_rect[3].position = qpl::vec(0, dimension.y);

		this->background_rect[0].color = qpl::rgb(10, 10, 50);
		this->background_rect[1].color = qpl::rgb(10, 10, 50);
		this->background_rect[2].color = qpl::rgb(0, 0, 0);
		this->background_rect[3].color = qpl::rgb(0, 0, 0);
	}
};
