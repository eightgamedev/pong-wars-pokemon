# pragma once
# include "Common.hpp"

class Setting : public App::Scene
{
public:
	Setting(const InitData& init);

	void update() override;
	void draw() const override;
};
