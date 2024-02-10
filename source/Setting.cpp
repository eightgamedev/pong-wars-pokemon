# include "Setting.hpp"

Setting::Setting(const InitData& init)
	: IScene(init)
{
}

void Setting::update()
{
	m_singleTypeWarButton.update();
	m_multipleTypeWarButton.update();
	if (m_singleTypeWarButton.isPushed())
	{
		changeScene(State::SingleTypeWar);
	}
	if (m_multipleTypeWarButton.isPushed())
	{
		changeScene(State::MultipleTypeWar);
	}
}

void Setting::draw() const
{
	m_singleTypeWarButton.draw();
	m_multipleTypeWarButton.draw();
}
