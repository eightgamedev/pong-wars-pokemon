# pragma once
# include "Common.hpp"
# include "Button.hpp"

class Setting : public App::Scene
{
public:
	Setting(const InitData& init);

	void update() override;
	void draw() const override;
private:
	CustomizedSimpleGUI::Button m_singleTypeWarButton{ U"Single Type War", 100, Vec2{ 100, 100 } };
	CustomizedSimpleGUI::Button m_multipleTypeWarButton{ U"Multiple Type War", 100, Vec2{ 100, 300 } };
};
