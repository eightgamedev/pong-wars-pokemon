# include "Setting.hpp"

Setting::Setting(const InitData& init)
	: IScene(init)
{
}

void Setting::update()
{
	m_singleTypeWarButton.update();
	m_multipleTypeWarButton.update();
	m_switchScreenModeButton.update();
	if (m_singleTypeWarButton.isPushed())
	{
		changeScene(State::SingleTypeWar);
	}
	if (m_multipleTypeWarButton.isPushed())
	{
		changeScene(State::MultipleTypeWar);
	}
	if (m_switchScreenModeButton.isPushed())
	{
		if (Window::GetState().fullscreen)
		{
			Window::SetFullscreen(false);
		}
		else
		{
			Window::SetFullscreen(true);
		}
	}
}

void Setting::draw() const
{
	m_singleTypeWarButton.draw();
	m_multipleTypeWarButton.draw();
	m_switchScreenModeButton.draw();
}
